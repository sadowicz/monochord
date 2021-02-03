#include "parse.h"

void parseArgs(int argc, char** argv, int* dataSig, int* cmdSig, char** binPath, char** txtPath)
{
    int opt;

    while((opt = getopt(argc, argv, ":b:t:d:c:")) != -1)
    {
        switch(opt)
        {
            case 'b':
                initPath(binPath, optarg);
                break;
            case 't':
                initPath(txtPath, optarg);
                break;
            case 'd':
                *dataSig = strToRtSig(optarg);
                break;
            case 'c':
                *cmdSig = strToRtSig(optarg);
                break;
            case ':':
                errExit("parseArgs: Argument needs value.");
            case '?':
                errExit("parseArgs: Unrecognized argument.");
        }
    }

    if(!*txtPath)   // init txtPath with default value if no value was given in argv
        initPath(txtPath, "-");

    if(*dataSig == -1 || *cmdSig == -1)
        errExit("parseArgs: Missing non optional argument.");

    if(*dataSig == *cmdSig)
        errExit("parseArgs: Signals cannot have same value.");

}

void initPath(char** path, char* value)
{
    *path = (char*)calloc(strlen(value) + 1, sizeof(char));
    if(!*path)
        errExit("initPath: Unable to allocate file path buffer");

    memcpy(*path, value, strlen(value));
}

int strToRtSig(char* str)
{
    char* endptr = NULL;
    errno = 0;

    long res = strtol(str, &endptr, 10);

    if(errno || *endptr || res < SIGRTMIN || res > SIGRTMAX)
        errExit("strToRtSig: Unable to convert string to real-time signal number");

    return (int)res;
}