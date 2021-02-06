#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

typedef struct Info {
    int recordingActive;
    int refPointUsed;
    int pidUsed;
    int binUsed;
}Info;

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void registerSignalHandler(int signal, void (*handler)(int, siginfo_t*, void *));
void sendInfoRequest(int signal, pid_t pid);
int collectInfo(int signal);

void decodeInfo(int codedInfo, Info* decodedInfo);

void parseArgs(int argc, char** argv, int* sig, pid_t* pid);
int strToInt(char* str);
int strToRtSig(char* str);

#endif
