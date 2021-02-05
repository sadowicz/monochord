#include <stdio.h>

#include "utils.h"
#include "parse.h"

//TODO: Block data signal until finished all actions based on it (eg. in while)
//TODO: Block cmd signal until finished all actions based on it (eg. in while)

void dataSignalNotifier(int signo, siginfo_t* siginfo, void* data);
void cmdSignalNotifier(int signo, siginfo_t* siginfo, void* data);

SigInfo dataSigInfo = {0};
SigInfo cmdSigInfo = {0};

int main(int argc, char* argv[])
{
    if(argc < 5 || argc > 9)
        usageExit(argv[0], "-d <int> -c <int> [-b <path> -t <path>]");

    int dataSig = -1;
    int cmdSig = -1;
    char* binPath = NULL;
    char* txtPath = NULL;

    ProgramFlags flags = {1, 0, 0, 0, 0, 0, 0, 0};

    parseArgs(argc, argv, &dataSig, &cmdSig, &binPath, &txtPath, &flags.useBin);

    int txtFd;
    int binFd;

    txtFd = openFile(txtPath);
    if(flags.useBin)
        binFd = openFile(binPath);

    struct timespec refPoint;

    //registerSignalHandler(dataSig, dataSignalNotifier);
    ignoreSignal(dataSig);  // instead of blocking, for signal not be queued
    registerSignalHandler(cmdSig, cmdSignalNotifier);

    while(1)
    {
        pause();

        if(cmdSigInfo.notified)
        {
            cmdSigInfo.notified = 0;

            decodeCmd(cmdSigInfo.data, &flags);

            if(flags.stopped)
            {
                ignoreSignal(dataSig);  // instead of blocking, for signal not be queued
            }
            else if(flags.sendInfo)
            {
                flags.sendInfo = 0;
                int info = codeFlags(&flags);
                sendInfo(cmdSigInfo.senderPid, cmdSig, info);
            }
            else    // Start command
            {
                if(isSignalIgnored(dataSig)) // reset handler function (start handling)
                    registerSignalHandler(dataSig, dataSignalNotifier);

                if(flags.truncFiles)
                {
                    flags.truncFiles = 0;
                    truncFile(txtFd);
                    if(flags.useBin)
                        truncFile(binFd);
                }

                // update refpoint or create one if old ref is demanded and it not exists:
                if(flags.updateRefPoint ||(flags.useRefPoint && !flags.hasRefPoint))
                {
                    flags.updateRefPoint = 0;
                    getTimestamp(CLOCK_MONOTONIC, &refPoint);
                }
            }

            //TODO: Unlock cmd signal handling here
        }

        if(dataSigInfo.notified)
        {
            dataSigInfo.notified = 0;
            float value;
            memcpy(&value, (void*)&dataSigInfo.data, sizeof(float));    // way to interpret non float 4-byte as float

            pid_t pid = -1;

            if(flags.usePid)
                pid = dataSigInfo.senderPid;

            struct timespec timestamp;

            if(flags.useRefPoint)
            {
                struct timespec temp;
                getTimestamp(CLOCK_MONOTONIC, &temp);
                getTimestampDiff(&timestamp, &refPoint, &temp);
            }
            else
                getTimestamp(CLOCK_REALTIME, &timestamp);

            writeRecordTxt(txtFd, &timestamp, value, pid, &flags);

            if(flags.useBin)
                writeRecordBin(binFd, &timestamp, &value, &pid);

            //TODO: Unlock data signal handling here

        }

    }

    // No need to close files here because the only way to exit is to terminate

    return 0;
}

void dataSignalNotifier(int signo, siginfo_t* siginfo, void* data)
{
    dataSigInfo.notified = 1;
    dataSigInfo.data = siginfo->si_value.sival_int;
    dataSigInfo.senderPid = siginfo->si_pid;

    //TODO: Lock data signal handling here
}

void cmdSignalNotifier(int signo, siginfo_t* siginfo, void* data)
{
    cmdSigInfo.notified = 1;
    cmdSigInfo.data = siginfo->si_value.sival_int;
    cmdSigInfo.senderPid = siginfo->si_pid;

    //TODO: Lock cmd signal handling here
}