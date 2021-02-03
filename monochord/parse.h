#ifndef PARSE_H
#define PARSE_H

#define _ISOC99_SOURCE
#define _POSIX_C_SOURCE 200809L

#define MAX_RECORD_LEN 128
#define MAX_NAME_LEN 8
#define MAX_VALUE_LEN 50

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

void interpretDatagram(char* datagram, SimParams* params, SimFlags* flags);
void splitRecord(char* record, char* name, char* value);
void executeRecord(char* name, char* value, SimParams* params, SimFlags* flags);

short strToPort(char* str);
float strToFloat(char* str);
float strToPosFloat(char* str);
int strToInt(char* str);

#endif
