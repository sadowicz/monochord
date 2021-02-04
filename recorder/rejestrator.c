#include <stdio.h>

#include "utils.h"
#include "parse.h"

void dataSignalNotifier(int signo, siginfo_t* siginfo, void* data);
void cmdSignalNotifier(int signo, siginfo_t* siginfo, void* data);

DataSig dataSigInfo = {0, 0.0f};
CmdSig cmdSigInfo = {0, 0, 0};

int main(int argc, char* argv[])
{
    if(argc < 5 || argc > 9)
        usageExit(argv[0], "-d <int> -c <int> [-b <path> -t <path>]");

    int dataSig = -1;
    int cmdSig = -1;
    char* binPath = NULL;
    char* txtPath = NULL;

    parseArgs(argc, argv, &dataSig, &cmdSig, &binPath, &txtPath);

    //printf("DATA: %d\nCMD: %d\nBIN: %s\nTXT: %s\n", dataSig, cmdSig, binPath, txtPath);

    return 0;
}

void dataSignalNotifier(int signo, siginfo_t* siginfo, void* data)
{
    dataSigInfo.notified = 1;
    memcpy(&dataSigInfo.data, &siginfo->si_value.sival_int, sizeof(float)); // copy int to float to interpret its value as float
}

void cmdSignalNotifier(int signo, siginfo_t* siginfo, void* data)
{
    cmdSigInfo.notified = 1;
    cmdSigInfo.data = siginfo->si_value.sival_int;
    cmdSigInfo.senderPid = siginfo->si_pid;
}