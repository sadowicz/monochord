#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE

#define MAX_TIMESTAMP_LEN 64

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct SigInfo {
    volatile sig_atomic_t notified;
    volatile sig_atomic_t data;
    volatile pid_t senderPid;
}SigInfo;

typedef struct ProgramFlags {
    int stopped;
    int useRefPoint;
    int updateRefPoint;
    int usePid;
    int useBin;
}ProgramFlags;

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void registerSignalHandler(int signal, void (*handler)(int, siginfo_t*, void *));
void sendInfo(pid_t pid, int signal, int info);

int openFile(char* path);

int codeFlags(ProgramFlags* flags);

void writeRecordBin(int fd, struct timespec* timestamp, float* data, pid_t* pid);

void writeTimestampGlobal(int fd);
void writeTimestampLocal(int fd, struct timespec* refPoint);
void getTimestampDiff(struct timespec* res, struct timespec* earlier, struct timespec* later);

#endif
