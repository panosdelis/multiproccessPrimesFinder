//
// Created by panos on 14/11/2020.
//
# include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "linkedList.h"
#include <sys/times.h> /* times () */
#include <unistd.h>
#include "primes.h"


# define YES 1
# define NO 0

int prime0 (int n){
    int i=0 , limitup =0;
    limitup = ( int )( sqrt (( double )n));
    if (n ==1) return (NO);
    for (i=2 ; i <= limitup ; i ++)
        if ( n % i == 0) return (NO);
    return ( YES );
}

int prime1(int n){
    int i;
    if (n ==1) return (NO);
    for (i=2 ; i<n ; i++)
        if ( n % i == 0) return (NO);
    return ( YES );
}

int prime2(int n){
    int flag = 0;
    for (int i = 2; i <= n / 2; ++i) {
        // condition for non-prime numbers
        if (n % i == 0) {
            flag = 1;
            break;
        }
    }

    if (n!=1){
        if (flag == 0) {
            return 1;
            //printf("%d is a prime number.", n);
        }else {
            return 0;
            //printf("%d is not a prime number.", n);
        }
    }
    //to stop the whining
    return 0;
}

int prime( int lb , int ub, int childNum, LinkedList** linkedList) {
    double t1 , t2 ;
    struct tms tb1 , tb2 ;
    double ticspersec ;
    ticspersec = ( double ) sysconf ( _SC_CLK_TCK );
    t1 = ( double ) times (& tb1) ;
    double realTime;

    int (*primeFunc)(int);
    if(childNum%3==0){
        primeFunc=&prime0;
    }else if(childNum%3==1){
        primeFunc=&prime1;
    }else{
        primeFunc=&prime2;
    }
    int i=0;
    if ( ( lb <1 ) || ( lb > ub ) ) {
        //printf (" usage : prime1 lb ub\n") ;
        return (-1) ; }
    for (i=lb ; i < ub ; i ++) {
        if (primeFunc(i) == YES) {
            LinkedNode *tempNode = (LinkedNode *) malloc(sizeof(LinkedNode));
            tempNode->number = i;
            t2 = (double) times(&tb2);
            realTime = (t2 - t1) / ticspersec;
            ticspersec = ( double ) sysconf ( _SC_CLK_TCK );
            t1 = ( double ) times (& tb1) ;

            tempNode->findTime = (char *) malloc(50);
            snprintf(tempNode->findTime, 50, "%f", realTime);
            //strcpy(tempNode->findTime,"1312");
            tempNode->next = NULL;
            if (*linkedList == NULL) {
                LinkedList *tempList = (LinkedList *) malloc(sizeof(LinkedList));
                tempList->start = tempNode;
                tempList->end = tempNode;
                *linkedList = tempList;
            } else {
                (*linkedList)->end->next = tempNode;
                (*linkedList)->end = tempNode;
            }
            //printf("%d ", i);
        }
    }
    //printf ("\n") ;
    return 0;
}