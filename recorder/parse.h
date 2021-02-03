#ifndef PARSE_H
#define PARSE_H

#define _POSIX_C_SOURCE 2
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include "utils.h"

void parseArgs(int argc, char** argv, int* dataSig, int* cmdSig, char** binPath, char** txtPath);

void initPath(char** path, char* value);
int strToRtSig(char* str);

#endif
