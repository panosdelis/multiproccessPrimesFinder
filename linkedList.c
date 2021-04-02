#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

void addToList(LinkedList** list, int number, char* findTime){
    LinkedNode* toAdd;
    toAdd = (LinkedNode *) malloc(sizeof(LinkedNode));
    toAdd->number = number;
    toAdd->next = NULL;
    toAdd->findTime=(char*)malloc(50);
    strcpy(toAdd->findTime,findTime);
    toAdd->numTime=atof(findTime);

    //check if is first
    if ((*list) == NULL){
        (*list) = (LinkedList *) malloc(sizeof(LinkedList));
        (*list)->start = toAdd;
        (*list)->end = toAdd;
        return;
    }
    if((*list)->start->number > toAdd->number){
        toAdd->next=(*list)->start;
        (*list)->start=toAdd;
        return;
    }

    //add it at the right spot
    //int done = 0;
    LinkedNode* tempNode;
    LinkedNode* prev;
    prev=NULL;
    tempNode=(*list)->start;
    while(tempNode!= NULL){
        if(tempNode->number > toAdd->number){
            //done=1;
            toAdd->next=tempNode;
            prev->next=toAdd;
            return;
        }
        if(tempNode->next==NULL){
            tempNode->next=toAdd;
            break;
        }
        prev=tempNode;
        tempNode=tempNode->next;
    }
}

void printList(LinkedList* linkedList){
    LinkedNode* tempNode;
    tempNode=linkedList->start;
    while (tempNode!=NULL){
        printf("%d ",tempNode->number);
        //printf("%s ",tempNode->findTime);
        printf("%.f ",tempNode->numTime*1000);
        tempNode=tempNode->next;
    }
}

void freeNode(LinkedNode* tempNode){
    if(tempNode==NULL)
        return;
    freeNode(tempNode->next);
    free(tempNode->findTime);
    free(tempNode);
}

void freeList(LinkedList** list){
    LinkedNode* tempNode;
    tempNode=(*list)->start;
    freeNode(tempNode);
    free(*list);
}