LDFLAGS=-lm

all: myprime  intermediate  worker

myprime: main.o pipes.o linkedList.o
	gcc -g -Wall -ggdb3 main.c pipes.c linkedList.c -o myprime $(LDFLAGS)

intermediate: intermediate.o pipes.o linkedList.o
	gcc -g -Wall -ggdb3 intermediate.c pipes.c linkedList.c -o intermediate $(LDFLAGS)

worker: worker.o pipes.o linkedList.o primes.o
	gcc -g -Wall -ggdb3 worker.c pipes.c linkedList.c primes.c -o worker $(LDFLAGS)