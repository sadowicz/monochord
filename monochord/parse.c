#include "parse.h"

void interpretDatagram(char* datagram, SimParams* params)
{
    char nameBuf[MAX_NAME_LEN] = {0};
    char valueBuf[MAX_VALUE_LEN] = {0};

    splitRecord(datagram, nameBuf, valueBuf);

    //printf("Received: %s\nName: %s\nValue: %s\n\n", datagram, nameBuf, valueBuf);
}

void splitRecord(char* datagram, char* name, char* value)
{
    char* readPtr = datagram;
    int currRead;

    if(!sscanf(datagram, "%[^: \t]%n", name, &currRead))
        errExit("splitRecord: Unable to get record name");

    readPtr += currRead;
    while(*readPtr == ' ' || *readPtr == '\t' ||*readPtr == ':')
        readPtr++;

    if(readPtr != datagram + strlen(datagram))
        if(!sscanf(readPtr, "%s", value))
            errExit("splitRecord: Unable to get record value");
}

short strToPort(char* str)
{
    char* endptr = NULL;
    errno = 0;

    long res = strtol(str, &endptr, 10);

    if(errno || *endptr || res < 0 || res > 32767)
        errExit("strToPort: Unable to convert string to port number");

    return (short)res;
}

float strToFloat(char* str)
{
    char* endptr = NULL;
    errno = 0;

    float res = strtof(str, &endptr);

    if(errno || *endptr)
        errExit("strToFloat: Unable to convert string to float number");

    return res;
}

float strToPosFloat(char* str)
{
    float res = strToFloat(str);
    if(res < 0)
        errExit("strToPosFloat: Converted string contains negative float number");

    return res;
}

int strToInt(char* str)
{
    char* endptr = NULL;
    errno = 0;

    long res = strtol(str, &endptr, 10);

    if(errno || *endptr)
        errExit("strToInt: Unable to convert string to int number");

    return (int)res;
}