# Project 2 Operating Systems Multiproccess Primes Finder
# Panagiotis Delis

# A multiproccess program that finds prime numbers between two values.

The work consists of 11 files:

Source: main.c, intermediate.c, worker.c, primes.c, pipes.c, linkedList.c

Header: primes.h, pipes.h, linkedList.h

Makefile: Makefile

README: This is the file here.

-------------------------------------------------- -------------------
Compile:
Make

Execute:
./myprime -l lb -u ub -w NumofChildren

They can be entered in any order as long as the correct flag is before the corresponding variable.


In main some variables are checked from the terminal and react accordingly.

e.g. For lower boundary numbers = <0 we replace with 1.

If more workers are required than numbers for control we reduce the number of processes.

-------------------------------------------------- -------------------
Processes and Pipes:

After the beginning of the main we create with fork NumberOfChildren processes and execute the intermediate using execl.
Respectively the intermediate after the necessary actions again with fork executes with execl NumberOfChildren workers.

The processes communicate with each other through named-pipes. For each relationship between processes a read pipe and a write pipe. Use my custom readFromPipe writeToPipe functions based on read write.
The pipe buffer can be found in pipes.h and we can change it.
The default is #define MSG_BUF 100.

To understand that a process has finished its write to the parent, we send a "stop" message via pipe.

In order not to have busy waiting we have polls in every program that expects results from its children with NumberOfChildren fds.

-------------------------------------------------- ---------------------
Prime Numbers:

To find prime numbers, we use 3 different functions, from pronunciation with minor changes and one of my own.

The basic prime is a function which, depending on the number of the worker, chooses which of the three methods will be used to find primes.
Passes function pointer and calls the corresponding all have the same format variables.

-------------------------------------------------- ---------------------
Data Structures:

For the storage of the prime numbers that have been found as well as for the time of finding them, we have the linked list structure, where during each program we keep them there and then we transfer them node - node to the respective parent.

-------------------------------------------------- ----------------------
Signals:

It sends at the end of each worker to the root node directly and with a signal handler we count how many signals we received (use flag for the correct increase of the number).

-------------------------------------------------- ----------------------
Other details.

The work has been made on Ubuntu 18.04 and has been tested with Valgrind at each level without leaks or other problems, it has been tested accordingly on the school machines.
