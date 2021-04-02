//
// Created by panos on 13/11/2020.
//
#ifndef PROJECT2_PIPES_H
#define PROJECT2_PIPES_H
#include "linkedList.h"
#define MSG_BUF 100

void CreatePipes(char* readFifo ,char* writFifo, char** childToParent , char** parentToChild , int NumOfChildren);
void DeletePipes(char** childToParent , char** parentToChild ,int NumOfChildren);
int numPlaces (int n);
void writeToPipe(int fd, char* pipe, char* str,int buffer);
int readFromPipe(int fd, char* pipe,char** aString ,int buffer);
#endif //PROJECT2_PIPES_H
