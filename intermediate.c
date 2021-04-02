//
// Created by panos on 12/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <math.h>
#include <sys/poll.h>
#include <poll.h>
#include <signal.h>

#include "pipes.h"
int main(int argc , char* argv[]){
    int status = 0;

    int NumOfChildren = atoi(argv[1]);
    int childNumber = atoi (argv[2]);
    //printf("NUM= %d\n",NumOfChildren);
    //exit(1);
    //printf("child= %d\n",childNumber);

    char toRead[strlen(argv[3])+1];
    strcpy(toRead,argv[3]);

    char toWrit[strlen(argv[4])+1];
    strcpy(toWrit,argv[4]);

    //pid_t root = atoi(argv[5]);

    int toWritFd = open(toWrit, O_WRONLY);
    int toReadFd = open(toRead, O_RDONLY);
//    int rFd = open(toRead, O_RDONLY);
//    int wFd = open(toWrit, O_WRONLY);

    int digits=(int)(floor (log10 (abs (NumOfChildren))) + 2);
    char* childNumText=(char*)malloc(digits+1);
    sprintf(childNumText, "%d", childNumber);
    char* childToParent[NumOfChildren];
    char* parentToChild[NumOfChildren];
    char* readFifo=(char*)malloc(5+digits);
    strcpy(readFifo,"read");
    strcat(readFifo,childNumText);
    strcat(readFifo,"I");
    char* writFifo=(char*)malloc(5+digits);
    strcpy(writFifo,"writ");
    strcat(writFifo,childNumText);
    strcat(writFifo,"I");
    free(childNumText);
//    char* writFifo="writXI";
//    readFifo[4]= (childNumber+'0');
//    writFifo[4]= (childNumber+'0');
    CreatePipes(readFifo ,writFifo, childToParent ,parentToChild ,NumOfChildren);

    char* num0;
    char* num1;
    readFromPipe(toReadFd,toRead,&num0,MSG_BUF);
    readFromPipe(toReadFd,toRead,&num1,MSG_BUF);
    //printf("numbers are : %s , %s\n", num0, num1 );

    int lb=atoi(num0);
    int ub=atoi(num1);

    int wFd[NumOfChildren];
    int rFd[NumOfChildren];


    int done=0;
    pid_t pid;
    //pid_t childrenPids[NumOfChildren];
    //char* childNumText;
    int gap=ub-lb;
    int part=gap/NumOfChildren;
    if (gap%2==1) part+=1; // add 1 to part if gap/children is odd number

    int startNum=lb;
    int endNum=startNum+part;
    char* startText;
    char* endText;

    for(int i=0; i < NumOfChildren; i++){
        if (i!=0)
            childNumText=(char*)malloc((int)(floor (log10 (abs (i))) + 2));
        else
            childNumText=(char*)malloc(2);
        sprintf(childNumText, "%d", i);
        startText=(char*)malloc((int)(floor (log10 (abs (startNum))) + 2));
        endText=(char*)malloc((int)(floor (log10 (abs (endNum))) + 2));
        sprintf(startText, "%d", startNum);
        sprintf(endText, "%d", endNum);
        pid = fork();
        if (pid==0) {
            sprintf(childNumText, "%d", i);
            sprintf(startText, "%d", startNum);
            sprintf(endText, "%d", endNum);
            execl("worker", "worker", parentToChild[i], childToParent[i], childNumText, argv[5] , NULL);
//            char forGDB[1000];
//            //strcpy(forGDB, "gdb -ex run --args worker");
//            strcpy(forGDB, "valgrind --leak-check=full --track-origins=yes --log-file=valgrind-out.txt ./worker");
//            //strcpy(forGDB, "./intermediate");
//            strcat(forGDB," ");
//            strcat(forGDB,parentToChild[i]); strcat(forGDB," "); strcat(forGDB,childToParent[i]); strcat(forGDB," ");
//            strcat(forGDB,childNumText); strcat(forGDB," "); strcat(forGDB,argv[5]);
//            execl("/usr/bin/gnome-terminal","gnome-terminal", "-e", forGDB, NULL);
            perror("EXEC \n");
        }
        rFd[i] = open(childToParent[i], O_RDONLY);
        wFd[i] = open(parentToChild[i], O_WRONLY);

        writeToPipe(wFd[i],parentToChild[i], startText,MSG_BUF);
        writeToPipe(wFd[i],parentToChild[i], endText, MSG_BUF);
        startNum+=part;
        endNum+=part;

        //childrenPids[i]=pid;
        free(childNumText);
        free(startText);
        free(endText);

        if(done==1)
            break;
        if(i==NumOfChildren-2){
            endNum=ub;
        }else if(endNum>=ub){
            endNum=ub;
            //NumOfChildren=i+1;
            done=1;
        }

    }

    char* runTimes[NumOfChildren];
//    for(int z=0;z<NumOfChildren;z++)
//        runTimes[z]=NULL;
    int endTimes[NumOfChildren];
    LinkedList* linkedList=NULL;
    LinkedNode *tempNode;
    int ended[NumOfChildren];
    for(int i=0;i<NumOfChildren;i++) {
        ended[i] = 0;
        endTimes[i] = 0;
    }

    int rc;
    //LETS POLL
    struct pollfd fds[NumOfChildren];
    memset (fds, 0, sizeof(fds));
    for(int i=0; i< NumOfChildren ; i++) {
        fds[i].fd = rFd[i];
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }
    done=0;
    while(done==0){
/* initialize poll parameters */
//        for(int i=0; i< NumOfChildren ; i++) {
//            fds[i].fd = rFd[0];
//            fds[i].events = POLLIN;
//            fds[i].revents = 0;
//        }
/* wait for incomign data or poll timeout */
        rc = poll ( fds , NumOfChildren , 3* 60 *1000);
        //printf ( " STILL \n");
        if ( rc == 0 || rc < 0) {
            printf ( " Poll timed - out .\n " );
            break;
            //exit (1);
        }else if ( rc >= 1){
            //printf ( " STILL \n");
            for(int i=0;i<NumOfChildren; i++ ) {
                if (fds[i].revents & POLLIN) {
                    char *tempStr;
                    readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                    if( strcmp(tempStr,"stop")==0 ){
                        free(tempStr);
                        readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                        endTimes[i]=1;
                        runTimes[i]=(char*)malloc(strlen(tempStr)+1);
                        strcpy(runTimes[i],tempStr);
                        done=1;
                        for (int y = 0; y < NumOfChildren; y++) {
                            if (endTimes[y] == 0)
                                done = 0;
                        }
                    }else {
                        int tempNum = atoi(tempStr);
                        free(tempStr);
                        readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                        //printf("%s", tempStr);
                        addToList(&linkedList, tempNum, tempStr);
                    }

                    free(tempStr);

                }else {
                    if(fds[i].revents & POLLHUP) {
                        ended[i] = 1;
                        done = 1;
                        for (int y = 0; y < NumOfChildren; y++) {
                            if (ended[y] == 0)
                                done = 0;
                        }
                    }
                    //printf("%d", fds[i].revents);
                }
            }
        }
    }

    if(linkedList!=NULL) {
        //TODO HERE IS THE PROBLEM
        tempNode=linkedList->start;
        char *tempNum;
        while (tempNode != NULL) {
            tempNum = (char *) malloc((int) (floor(log10(abs(tempNode->number))) + 2));
            sprintf(tempNum, "%d", tempNode->number);
            //printf("%d ",tempNode->number);
            writeToPipe(toWritFd, toWrit, tempNum, MSG_BUF);
            writeToPipe(toWritFd, toWrit, tempNode->findTime, MSG_BUF);
            free(tempNum);
            tempNode = tempNode->next;
        }
    }

    writeToPipe(toWritFd,toWrit,"stop",MSG_BUF);

    for(int i=0;i<NumOfChildren;i++){
        writeToPipe(toWritFd,toWrit,runTimes[i],MSG_BUF);
        close(rFd[i]);
        close(wFd[i]);
        free(runTimes[i]);
    }
    close(toReadFd);
    close(toWritFd);

    while ((pid=waitpid(-1,&status,0))!=-1) {
        //printf("Process %d terminated\n",pid);
    }

    //sleep(3);//this should be removed


    DeletePipes(childToParent , parentToChild , NumOfChildren);
    freeList(&linkedList);
    free(num1);
    free(num0);
    free(readFifo);
    free(writFifo);
    //kill(root, SIGUSR1);
}