#include "parse.h"

void parseArgs(int argc, char** argv, int* dataSig, int* cmdSig, char** txtPath, char** binPath)
{
    int opt;

    while((opt = getopt(argc, argv, ":b:t:d:c:")) != -1)
    {
        switch(opt)
        {
            case 'b':
                *binPath = (char*)calloc(strlen(optarg) + 1, sizeof(char));
                if(!*binPath)
                    errExit("parseArgs: Unable to allocate binary file path buffer");

                memcpy(*binPath, optarg, strlen(optarg));
                break;
            case 't':
                *txtPath = (char*)calloc(strlen(optarg) + 1, sizeof(char));
                if(!*txtPath)
                    errExit("parseArgs: Unable to allocate text file path buffer");

                memcpy(*txtPath, optarg, strlen(optarg));
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

        if(!*txtPath)   // init txtPath with default value if no value was given in argv
        {
            *txtPath = (char*)calloc(2, sizeof(char));
            if(!*txtPath)
                errExit("parseArgs: Unable to allocate text file path buffer");

            memcpy(*txtPath, "-", 1);
        }
    }
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