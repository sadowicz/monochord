#include <stdio.h>

#include "utils.h"

int main(int argc, char* argv[])
{
    if(argc != 4)
        usageExit(argv[0], "-c <int> <int>");

    int sig;
    pid_t pid;

    parseArgs(argc, argv, &sig, &pid);


    return 0;
}