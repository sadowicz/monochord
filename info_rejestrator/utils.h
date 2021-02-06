#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void sendInfoRequest(int signal, pid_t pid);

void parseArgs(int argc, char** argv, int* sig, pid_t* pid);
int strToInt(char* str);
int strToRtSig(char* str);

#endif
