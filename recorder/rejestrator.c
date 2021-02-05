#include <stdio.h>

#include "utils.h"
#include "parse.h"

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

    ignoreSignal(dataSig);  // instead of blocking, for signal not be queued
    registerSignalHandler(cmdSig, cmdSignalNotifier);

    while(1)
    {
        pause();

        if(cmdSigInfo.notified)
        {
            sigset_t backup;
            blockSignal(cmdSig, &backup);  // block handling of cmd signals (enter "critical section")

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

            unblockSignal(cmdSig, &backup); // unblock handling of cmd signal (left "critical section")
        }

        if(dataSigInfo.notified)
        {
            sigset_t backup;
            blockSignal(dataSig, &backup);  // block handling of data signals (enter "critical section")

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

            unblockSignal(dataSig, &backup); // unblock handling of data signal (left "critical section")
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
}

void cmdSignalNotifier(int signo, siginfo_t* siginfo, void* data)
{
    cmdSigInfo.notified = 1;
    cmdSigInfo.data = siginfo->si_value.sival_int;
    cmdSigInfo.senderPid = siginfo->si_pid;
}