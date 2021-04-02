#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/poll.h>
#include <signal.h>

#include "pipes.h"

int signalsNum=0;
int increment=1;

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        while(increment==0){}
        increment=0;
        signalsNum++;
        increment=1;
        //printf("Received SIGUSR1!\n");
    }
    signal(SIGUSR1, my_handler);

}

//./myprime -l lb -u ub -w NumofChildren
int main(int argc, char *argv[]) {
    signal(SIGUSR1, my_handler);
    if (argc < 7) {
        printf("\nNot enough arguments \n");
        exit(EXIT_FAILURE);
    } else if (argc > 7) {
        printf("\nToo many arguments passed.\n");
        exit(EXIT_FAILURE);
    }
    int lb, ub , NumOfChildren;
    if(strcmp(argv[1],"-l")==0){
        if (strcmp(argv[3],"-u")==0){
            lb = atoi(argv[2]);
            ub = atoi(argv[4]);
            NumOfChildren = atoi(argv[6]);
        }else if(strcmp(argv[3],"-w")==0){
            lb = atoi(argv[2]);
            ub = atoi(argv[6]);
            NumOfChildren = atoi(argv[4]);
        }
    }else if(strcmp(argv[1],"-u")==0) {
        if (strcmp(argv[3],"-l")==0){
            lb = atoi(argv[4]);
            ub = atoi(argv[2]);
            NumOfChildren = atoi(argv[6]);
        }else if(strcmp(argv[3],"-w")==0){
            lb = atoi(argv[6]);
            ub = atoi(argv[2]);
            NumOfChildren = atoi(argv[4]);
        }
    }else if(strcmp(argv[1],"-w")==0) {
        if (strcmp(argv[3],"-l")==0){
            lb = atoi(argv[4]);
            ub = atoi(argv[6]);
            NumOfChildren = atoi(argv[2]);
        }else if(strcmp(argv[3],"-u")==0){
            lb = atoi(argv[6]);
            ub = atoi(argv[4]);
            NumOfChildren = atoi(argv[2]);
        }
    }else{
        printf("\nSomething wrong with arguments.\n");
        printf("Should be like ( in any order ) : -l lb -u ub -w NumofChildren\n");
        exit(EXIT_FAILURE);
    }

    if(NumOfChildren<=0){
        printf("Nothing can happen without workers. Give more than 0\n");
        exit(EXIT_FAILURE);
    }
    if(lb >= ub){
        printf("Lower Boundary greater than Upper Boundary\n");
        exit(EXIT_FAILURE);
    }



    //int status = 0;

    if (lb<=0){
        printf("Lower bound was wrong made it 1.\n");
        lb=1;
    }

    //NumOfChildren=((int)sqrt(NumOfChildren));
    //printf("NUMof %d\n",NumOfChildren);
    int gap=ub-lb;
    if(gap<=1){
        printf("Lower Boundary and Upper Boundary too close.\n");
        exit(EXIT_FAILURE);
    }

    int part=gap/NumOfChildren;
    //printf("GAP= %d\n",gap);
    if (gap%2==1) part+=1; // add 1 to part if gap/children is odd number
    //printf("gap/Num= %d\n",gap / (NumOfChildren*NumOfChildren));
    if( gap <= (NumOfChildren*NumOfChildren)){//TODO FIX THIS
        //printf("WHAT %d\n",gap / (NumOfChildren*NumOfChildren));
        NumOfChildren=((int)sqrt(gap))-1;
//        if(NumOfChildren<2)
//            NumOfChildren=2;
        part=gap/NumOfChildren;
        //printf("NUMof %d\n",NumOfChildren);
    }



    //exit(1);
    char* textNumOfChildren=(char *) malloc((int) (floor(log10(abs(NumOfChildren))) + 2));
    sprintf(textNumOfChildren, "%d", NumOfChildren);
    //printf ("%d , %d , %d\n",lb,ub,NumOfChildren);
    int done=0;
    char* childToParent[NumOfChildren];
    char* parentToChild[NumOfChildren];
    //char str[strlen(readFifo) + (int)(floor (log10 (abs (NumOfChildren))) + 1)];
    char readFifo[5];
    strcpy(readFifo,"read");
    //printf("ReadFF= %s\n",readFifo);
    char writFifo[5];
    strcpy(writFifo,"writ");

    int wFd[NumOfChildren];
    int rFd[NumOfChildren];
    CreatePipes(readFifo ,writFifo, childToParent ,parentToChild ,NumOfChildren);

    pid_t pid;
    //pid_t childrenPids[NumOfChildren];
    char* childNumText;


    int startNum=lb;
    int endNum=startNum+part;
    char* startText;
    char* endText;
    char* rootPid;
    rootPid=(char*)malloc((int)(floor (log10 (abs (getpid()))) + 2));
    sprintf(rootPid, "%d", getpid());

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
            execl("intermediate", "intermediate", textNumOfChildren, childNumText, parentToChild[i], childToParent[i], rootPid , NULL);
            char forGDB[1000];
//            //strcpy(forGDB, "gdb -ex run --args intermediate");
//            //strcpy(forGDB, "valgrind --leak-check=full --track-origins=yes --log-file=valgrind-out.txt ./intermediate");
//            //strcpy(forGDB, "./intermediate");
//            strcat(forGDB," "); strcat(forGDB,textNumOfChildren); strcat(forGDB," "); strcat(forGDB,childNumText); strcat(forGDB," ");
//            strcat(forGDB,parentToChild[i]); strcat(forGDB," "); strcat(forGDB,childToParent[i]); strcat(forGDB," ");
//            strcat(forGDB,rootPid);
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
            done=1;
            //NumOfChildren=i+1;
        }

    }

    double max=-1;
    double min=-1;
    //char* runTimes[NumOfChildren][NumOfChildren];
    double runTimes[NumOfChildren][NumOfChildren];
    //int endTimes[NumOfChildren];
    //sleep(10);//this should be removed
    LinkedList* linkedList=NULL;
    int currentNum=NumOfChildren;
    int ended[NumOfChildren];
    for(int i=0;i<NumOfChildren;i++) {
        ended[i] = 0;
        //endTimes[i] = 0;
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
        rc = poll ( fds , currentNum , 3 * 60* 1000);
        //printf ( " STILL \n");
        if ( rc == 0 ) {
            printf ( " Poll timed - out .\n " );
            break;
            //exit (1);
        }else if (rc < 0){
            continue;
            //printf("%d \n",rc);
            //exit(17);
        }else if ( rc >= 1){
            //printf ( " STILL \n");
            for(int i=0;i<currentNum; i++ ) {
                if (fds[i].revents & POLLIN) {
                    char *tempStr;
                    readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                    if( strcmp(tempStr,"stop")==0 ){
                        free(tempStr);
                        //readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                        //endTimes[i]=1;
                        for(int z=0;z<NumOfChildren;z++) {
                            readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                            //runTimes[i][z] = (char *) calloc(1,strlen(tempStr) + 1);
                            //strcpy(runTimes[i][z], tempStr);
                            runTimes[i][z] = atof(tempStr);
                            free(tempStr);
                        }
                    }else {
                        int tempNum = atoi(tempStr);
                        free(tempStr);
                        readFromPipe(rFd[i], " ", &tempStr, MSG_BUF);
                        addToList(&linkedList, tempNum,tempStr);
//                        if (max==-1 || min==-1) {
//                            max = atof(tempStr);
//                            min = atof(tempStr);
//                        }
//                        if(max < atof(tempStr))
//                            max = atof(tempStr);
//                        if(min > atof(tempStr))
//                            min = atof(tempStr);
                        free(tempStr);
                    }

                }else {
                    if(fds[i].revents & POLLHUP) {
                        ended[i]=1;
                        done = 1;
                        for(int y=0; y<NumOfChildren; y++){
                            if(ended[y]==0)
                                done=0;
                        }
                    }
                    //printf("%d", fds[i].revents);
                }
                //printf ( " %x", fds[i].revents);
            }
        }
    }


    //WAIT ALL CHILDREN
//    while ((pid=waitpid(-1,&status,0))!=-1) {
//        printf("Process %d terminated\n",pid);
//    }

    printf("\nPrimes in [%d,%d] are: ",lb,ub);
    printList(linkedList);
    //printf("\n \n \

    for(int i=0;i<NumOfChildren;i++){
        for(int z=0;z<NumOfChildren;z++){
            if (max==-1 || min==-1) {
                max = runTimes[i][z];
                min = runTimes[i][z];
            }
            if(max < runTimes[i][z])
                max = runTimes[i][z];
            if(min > runTimes[i][z])
                min = runTimes[i][z];
            //free(tempStr);
            //runTimes[i][z];
        }
    }
    printf("\nMin Time for Workers : %.f msecs\n", min*1000);
    printf("Max Time for Workers : %.f msecs\n", max*1000);

    printf("Num of USR1 Received : %d/%d: \n", signalsNum,(NumOfChildren*NumOfChildren));
    for(int i=0;i<NumOfChildren;i++){
        for(int z=0;z<NumOfChildren;z++){
            printf("Time for W%d: %.f msec\n", (i*NumOfChildren+z) , (runTimes[i][z]/*-floor(runTimes[i][z])*/)*1000 );
            //free(runTimes[i][z]);
        }
        close(rFd[i]);
        close(wFd[i]);
    }

    DeletePipes(childToParent , parentToChild , NumOfChildren);
    freeList(&linkedList);
    free(textNumOfChildren);
    free(rootPid);
    //printf("Num of USR1 Received : %d/%d: \n", signalsNum,(NumOfChildren*NumOfChildren));

}
