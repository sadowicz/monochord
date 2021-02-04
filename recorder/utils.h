#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 199309L
#define _BSD_SOURCE
#define _SVID_SOURCE

#define MAX_TIMESTAMP_LEN 32

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct DataSig {
    volatile sig_atomic_t notified;
    volatile float data;
}DataSig;

typedef struct CmdSig {
    volatile sig_atomic_t notified;
    volatile sig_atomic_t data;
    volatile pid_t senderPid;
}CmdSig;

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void registerSignalHandler(int signal, void (*handler)(int, siginfo_t*, void *));

int openFile(char* path);

void writeTimestampGlobal(int fd);
void writeTimestampLocal(int fd, struct timespec* refPoint);
void getTimestampDiff(struct timespec* res, struct timespec* earlier, struct timespec* later);

#endif
