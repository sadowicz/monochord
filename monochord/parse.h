#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"

short strToPort(char* str);
float strToFloat(char* str);
float strToPosFloat(char* str);
int strToInt(char* str);

#endif
