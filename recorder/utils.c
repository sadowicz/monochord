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