//
// Created by panos on 13/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<signal.h>

int numPlaces (int n) {
    //if (n < 0) return numPlaces ((n == INT_MIN) ? MAX_INT: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

void CreatePipes(char* readFifo ,char* writFifo, char** childToParent , char** parentToChild , int NumOfChildren){
    char readStr[strlen(readFifo) + numPlaces(NumOfChildren)];
    char writStr[strlen(readFifo) + numPlaces(NumOfChildren)];
    char str[numPlaces(NumOfChildren)];
    //printf("NUM???= %d\n",NumOfChildren);

    for (int i = 0; i < NumOfChildren; ++i) {
// Creating the named file(FIFO)
// mkfifo(<pathname>, <permission>)
        sprintf(str, "%d", i);
        strcpy(writStr,writFifo);
        strcpy(readStr,readFifo);
        strcat(writStr,str);
        strcat(readStr,str);
        mkfifo(readStr, 0666);
        mkfifo(writStr, 0666);
        childToParent[i] = (char *) malloc(strlen(readStr) + 1);
        strcpy(childToParent[i], readStr);
        //strcat(childToParent[i], "\0");
        parentToChild[i] = (char *) malloc(strlen(readStr) + 1);
        strcpy(parentToChild[i], writStr);
        //strcat(parentToChild[i],"\0");
    }

}

void DeletePipes(char** childToParent , char** parentToChild ,int NumOfChildren){
    for(int i=0;i<NumOfChildren;i++) {
        remove(childToParent[i]);
        free(childToParent[i]);
        remove(parentToChild[i]);
        free(parentToChild[i]);
//        if (!del || !del1 )
//            printf("The file is Deleted successfully\n");
//        else
//            printf("the file is not Deleted\n");
    }
}
//PRINT PIPES
//for(int i=0;i<NumOfChildren;i++){
//printf("readPipe %i=%s\n",i,childToParent[i]);
//printf("writePipe %i=%s\n",i,parentToChild[i]);
//}

void writeToPipe(int fd, char* pipe, char* str, int buffer) {
    char* tmp;
    char* size=(char*)calloc(1,8);
    tmp=(char*)malloc(8+1);
    char* toPush=(char*)malloc(sizeof(char) * buffer);
    int remainingBytes;
    sprintf(size, "%d", (int)strlen(str));
    strcat(size,"\n");
    strcpy(tmp,size);

    if(buffer>=8) {
        write(fd, size, 8);
    }else {
        remainingBytes = 8;
        while(remainingBytes>0) {
            memcpy(toPush, tmp, buffer);
            toPush[buffer]='\0';
            memset(tmp, '$', buffer);
            write(fd, toPush, buffer);
            tmp=strrchr(tmp, '$');
            tmp = strchr(tmp, tmp[1]);
            remainingBytes=remainingBytes-buffer;
        }
    }
    //strcpy(tmp,str);
    if(buffer>=(int)strlen(str)) {
        write(fd, str, (int)strlen(str));
    }else {
        remainingBytes = (int)strlen(str);
        while(remainingBytes>0) {
            memcpy(toPush, str, buffer);
            toPush[buffer]='\0';
            memset(str, '$', buffer);
            write(fd, toPush, buffer);
            str=strrchr(str, '$');
            if(str!=NULL)
                str=strchr(str, str[1]);
            else
                break;
            remainingBytes=remainingBytes-buffer;
        }
    }
    free(toPush);
    free(tmp);
    free(size);
}

int readFromPipe(int fd,char* pipe,char** aString, int buffer ) {
    int readReturn;

    char strSize[8];
    int size=1000;
    char* str;
    int done=0;//new
    int stop=0;
    int remainingBytes;//new
    char* toAppend;//new
    toAppend=(char*)malloc(buffer+1);;//new

    if(buffer>=8){//new
        readReturn = read(fd, strSize, 8);
    }else{//new
        remainingBytes = 8;
        while(remainingBytes>0&& stop==0) {
            readReturn=read(fd, toAppend, buffer);
            toAppend[buffer]='\0';
            if(done==0) {
                strcpy(strSize, toAppend);
                done=1;
            }else
                strcat(strSize,toAppend);
            remainingBytes=remainingBytes-buffer;
        }
    }
    size=atoi(strSize);
    str=(char*)malloc((size +1));
    *aString=(char*)malloc((size +1));
    //*aString=(char*)calloc((size +2),1);
    done=0;

    if(buffer>=size) {
        readReturn=read(fd, str, size);
    }else {
        remainingBytes = size;
        while (remainingBytes > 0) {
            readReturn=read(fd, toAppend, buffer);
            toAppend[buffer]='\0';
            if (done == 0) {
                strcpy(str, toAppend);
                done = 1;
            } else {
                strcat(str, toAppend);
            }
            remainingBytes = remainingBytes - buffer;
        }
    }
    str[size]='\0';
    strcpy(*aString,str);
    free(str);
    free(toAppend);
    return readReturn;
}