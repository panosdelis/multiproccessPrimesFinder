//
// Created by panos on 13/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/times.h>
#include <signal.h>

#include "primes.h"
#include "linkedList.h"
#include "pipes.h"

int main(int argc , char* argv[]) {

    double t1 , t2 ;
    struct tms tb1 , tb2 ;
    double ticspersec ;
    double primeTime=0;
    ticspersec = ( double ) sysconf ( _SC_CLK_TCK );
    t1 = ( double ) times (& tb1) ;
    //printf("Hello friend.\n");
    char toRead[strlen(argv[1])+1];
    strcpy(toRead,argv[1]);

    char toWrit[strlen(argv[2])+1];
    strcpy(toWrit,argv[2]);

    int childNum=atoi(argv[3]);

    pid_t root = atoi(argv[4]);

    int toWritFd = open(toWrit, O_WRONLY);
    int toReadFd = open(toRead, O_RDONLY);

    //freeeeee this
    char* num0;
    char* num1;
    readFromPipe(toReadFd,toRead,&num0,MSG_BUF);
    readFromPipe(toReadFd,toRead,&num1,MSG_BUF);
    //printf("Worker numbers are : %s , %s\n", num0, num1 );

    int lb=atoi(num0);
    int ub=atoi(num1);

    LinkedList* linkedList=NULL;

    prime(lb , ub, childNum, &linkedList);

    LinkedNode* tempNode;
    if(linkedList!=NULL) {
        tempNode = linkedList->start;
        char *tempNum;
        while (tempNode != NULL) {
            tempNum = (char *) malloc((int) (floor(log10(abs(tempNode->number))) +2));
            sprintf(tempNum, "%d", tempNode->number);
            //printf("%d ",tempNode->number);
            writeToPipe(toWritFd, toWrit, tempNum, MSG_BUF);
            writeToPipe(toWritFd, toWrit, tempNode->findTime, MSG_BUF);
            primeTime+=tempNode->numTime;
            free(tempNum);
            tempNode = tempNode->next;
        }
    }
    t2 = ( double ) times (& tb2) ;
    double realTime;
    char* timeText;
    timeText=(char*)malloc(51);
    //printf("%lf, %lf\n",t1,t2);
    realTime=(t2 - t1) / ticspersec;
    realTime+=primeTime;
    //printf("%lf\n",realTime);
    snprintf(timeText, 50, "%f", realTime);
    //HERE CALCULATE ELAPSED TIME
    writeToPipe(toWritFd,toWrit,"stop",MSG_BUF);
    writeToPipe(toWritFd,toWrit,timeText,MSG_BUF);
    free(timeText);

    //printList(linkedList);


    close(toReadFd);
    close(toWritFd);

    kill(root, SIGUSR1);
    freeList(&linkedList);
    free(num1);
    free(num0);
}
