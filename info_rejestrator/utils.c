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

void parseArgs(int argc, char** argv, int* sig, pid_t* pid)
{
    int opt;

    *sig = -1;

    while((opt = getopt(argc, argv, "-:c:")) != -1)
    {
        switch(opt)
        {
            case 'c':
                *sig = strToRtSig(optarg);
                break;
            case 1:
                *pid = strToInt(optarg);
                break;
            case ':':
                errExit("parseArgs: Argument needs value.");
            case '?':
                errExit("parseArgs: Unrecognized argument.");
        }
    }

    if(*sig == -1)
        errExit("parseArgs: Non optional -c argument not given.");
}

void registerSignalHandler(int signal, void (*handler)(int, siginfo_t*, void *))
{
    struct sigaction sa;

    if(sigemptyset(&sa.sa_mask))
        errExit("registerSignalHandler: Unable to empty sa_mask");

    if(sigaddset(&sa.sa_mask, signal))
        errExit("registerSignalHandler: Unable to add signal to sa_mask");

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    if(sigaction(signal, &sa, NULL))
        errExit("registerSignalHandler: Unable to register signal handler");
}

void sendInfoRequest(int signal, pid_t pid)
{
    union sigval val;
    val.sival_int = 255;

    if(sigqueue(pid, signal, val))
        errExit("sendInfoRequest: Unable to send signal with info request.");
}

int collectInfo(int signal)
{
    sigset_t set;
    siginfo_t siginfo;

    struct timespec timeout = {.tv_sec = 2, .tv_nsec = 0};

    if(sigemptyset(&set))
        errExit("collectInfo: Unable to empty set.");

    if(sigaddset(&set, signal))
        errExit("collectInfo: Unable to add signal to set.");

    errno = 0;

    if(sigtimedwait(&set, &siginfo, &timeout) == -1)
    {
        if(errno == EAGAIN)
        {
            fprintf(stderr, "No response catched before timeout.\n");
            exit(0);    // technically detecting timeout and informing about it is valid action (not ERROR) based on pdf
                        // so I'm using here fprintf and exiting with success (0) instead of errExit
        }
        else
            errExit("collectInfo: Unable to perform waiting for response");
    }

    return siginfo.si_value.sival_int;  // return coded info
}

void decodeInfo(int codedInfo, Info* decodedInfo)
{
    if(codedInfo & 0x1)     // 0001
        decodedInfo->recordingActive = 1;
    if(codedInfo & 0x2)     // 0010
        decodedInfo->refPointUsed = 1;
    if(codedInfo & 0x4)     // 0100
        decodedInfo->pidUsed = 1;
    if(codedInfo & 0x8)     // 1000
        decodedInfo->binUsed = 1;
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

int strToRtSig(char* str)
{
    int sig = strToInt(str);
    if(sig < SIGRTMIN || sig > SIGRTMAX)
        errExit("strToRtSig: Signal number out of range");

    return sig;
}
