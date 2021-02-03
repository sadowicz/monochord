#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void registerSignalHandler(int signal, void (*handler)(int, siginfo_t*, void *));

#endif
