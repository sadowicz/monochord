#ifndef PARSE_H
#define PARSE_H

#define MAX_RECORD_LEN 128
#define MAX_NAME_LEN 8
#define MAX_VALUE_LEN 50

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

void interpretDatagram(char* datagram, SimParams* params);
void interpretRecords(char* datagram, SimParams* params, int* reportFlag);
void splitRecord(char* record, char* name, char* value);
void executeRecord(char* name, char* value, SimParams* params, int* reportFlag);

short strToPort(char* str);
float strToFloat(char* str);
float strToPosFloat(char* str);
int strToInt(char* str);
int strToRtNumber(char* str);

#endif
