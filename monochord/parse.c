#include "parse.h"

short strToPort(char* str)
{
    char* endptr = NULL;
    errno = 0;

    long res = strtol(str, &endptr, 10);

    if(errno || *endptr || res < 0 || res > 32767)
        errExit("strToPort: Unable to convert string to port number");

    return (short)res;
}