#include "utils.h"

void errExit(const char* msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(1);
}

void usageExit(const char* programName, const char* msg)
{
    fprintf(stderr, "USAGE: %s %s\n", programName, msg);
    exit(1);
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

int strToSig(char* str)
{
    int sig = strToInt(str);
    if(sig <= 0 || sig > SIGRTMAX)
        errExit("strToSig: Signal number out of range");    // not specified if program must accept only rt signals

    return sig;
}
