#ifndef PARSE_H
#define PARSE_H

#define MAX_NAME_LEN 8
#define MAX_VALUE_LEN 50

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

void interpretDatagram(char* datagram, SimParams* params);
void splitRecord(char* datagram, char* name, char* value);

short strToPort(char* str);
float strToFloat(char* str);
float strToPosFloat(char* str);
int strToInt(char* str);

#endif
