#include <signal.h>
#include "parse.h"

void interpretDatagram(char* datagram, SimParams* params, SimFlags* flags)
{
    char recordBuf[MAX_RECORD_LEN] = {0};
    char nameBuf[MAX_NAME_LEN] = {0};
    char valueBuf[MAX_VALUE_LEN] = {0};

    char* readPtr = datagram;
    int totalRead = 0;
    int currRead = 0;

    while(totalRead < strlen(datagram))
    {
        if(!sscanf(readPtr, "%[^\n]%n", recordBuf, &currRead))
            errExit("interpretDatagram: Unable to get record");

        splitRecord(recordBuf, nameBuf, valueBuf);
        executeRecord(nameBuf, valueBuf, params, flags);

        readPtr += (currRead + 1);
        totalRead += (currRead + 1);

        memset(recordBuf, 0, MAX_RECORD_LEN);
        memset(nameBuf, 0, MAX_NAME_LEN);
        memset(valueBuf, 0, MAX_VALUE_LEN);
    }
}

void splitRecord(char* record, char* name, char* value)
{
    char* readPtr = record;
    int currRead;

    if(!sscanf(record, "%[^: \t]%n", name, &currRead))
        errExit("splitRecord: Unable to get record name");

    readPtr += currRead;
    while(*readPtr == ' ' || *readPtr == '\t' ||*readPtr == ':')
        readPtr++;

    if(readPtr != record + strlen(record))
        if(!sscanf(readPtr, "%s", value))
            errExit("splitRecord: Unable to get record value");
}

void executeRecord(char* name, char* value, SimParams* params, SimFlags* flags)
{
    if(!strcmp("amp", name))
    {
        params->amp = strToFloat(value);
        flags->reset = 1;
    }
    else if(!strcmp("freq", name))
    {
        params->freq = strToPosFloat(value);
        flags->reset = 1;
    }
    else if(!strcmp("probe", name))
    {
        params->probe = strToPosFloat(value);
        armTimer(params->timerId, params->probe);
    }
    else if(!strcmp("period", name))
    {
        params->period = strToFloat(value);

        if(params->period < 0) flags->stopped = 2;
        else if(!params->period) flags->stopped = 1;
        else flags->stopped = 0;
    }
    else if(!strcmp("pid", name))
    {
        params->pid = strToInt(value);
        flags->pidErr = 0;  // to try send signal when new pid is set
    }
    else if(!strcmp("rt", name))
    {
        params->rt = strToInt(value);
        flags->rtOutOfRange = (uint8_t)(params->rt > SIGRTMIN && params->rt < SIGRTMAX);
    }
    else if(!strcmp("raport", name))
        flags->report = 1;
    else
        errExit("executeRecord: Unrecognized record name");
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