#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 2
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void parseArgs(int argc, char** argv, int* sig, pid_t* pid);
int strToInt(char* str);
int strToSig(char* str);

#endif
