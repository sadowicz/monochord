#include <stdio.h>

#include "utils.h"

volatile sig_atomic_t infoRecv = 0;

void infoSignalNotifier(int signo, siginfo_t* siginfo, void* data);

int main(int argc, char* argv[])
{
    if(argc != 4)
        usageExit(argv[0], "-c <int> <int>");

    int sig;
    pid_t pid;
    Info info = {0, 0, 0, 0};

    parseArgs(argc, argv, &sig, &pid);
    registerSignalHandler(sig, infoSignalNotifier);

    sendInfoRequest(sig, pid);
    int codedResp =collectInfo(sig);
    decodeInfo(codedResp, &info);

    printInfo(&info);

    return 0;
}

void infoSignalNotifier(int signo, siginfo_t* siginfo, void* data)
{
    infoRecv = 1;
}