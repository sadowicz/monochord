#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE

#define MAX_RECORD_LEN 128

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
    int usePid;
    int useBin;
    int updateRefPoint;
    int truncFiles;
    int sendInfo;
}ProgramFlags;


void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void registerSignalHandler(int signal, void (*handler)(int, siginfo_t*, void *));
void sendInfo(pid_t pid, int signal, int info);

int codeFlags(ProgramFlags* flags);
void decodeCmd(int cmdData, ProgramFlags* flags);

int openFile(char* path);
void writeRecordTxt(int fd, struct timespec* timestamp, float data, pid_t* pid, ProgramFlags* flags);
void writeRecordBin(int fd, struct timespec* timestamp, float* data, pid_t* pid);

void getStrTimestampGlobal(struct timespec* timestamp, char* buffer);
void getStrTimestampLocal(struct timespec* timestamp, char* buffer);
void getTimestampDiff(struct timespec* res, struct timespec* earlier, struct timespec* later);

#endif
