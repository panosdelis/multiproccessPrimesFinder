//
// Created by panos on 15/11/2020.
//

#ifndef PROJECT2_LINKEDLIST_H
#define PROJECT2_LINKEDLIST_H
typedef struct linkedNode{
    int number;
    char* findTime;
    double numTime;
    struct linkedNode* next;
}LinkedNode;

typedef struct linkedList{
    struct linkedNode* start;
    struct linkedNode* end;
}LinkedList;

void addToList(LinkedList** list, int number, char* findTime);
void printList(LinkedList* linkedList);
void freeList(LinkedList** list);
#endif //PROJECT2_LINKEDLIST_H
