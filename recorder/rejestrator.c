#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>

#include "utils.h"
#include "parse.h"

void openFiles(int* txtFd, char* txtPath, int* binFd, char* binPath, int useBinFlag);

void runRecorder(int dataSig, int cmdSig, int txtFd, int binFd, ProgramFlags* flags);

void executeCmd(int txtFd, int binFd, int dataSig, int cmdSig, struct timespec* refPoint, ProgramFlags* flags);
void executeStartCmd(int txtFd, int binFd, int dataSig, struct timespec* refPoint, ProgramFlags* flags);

void saveData(int txtFd, int binFd, struct timespec* refPoint, ProgramFlags* flags);

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
    int txtFd;
    int binFd;
    ProgramFlags flags = {1, 0, 0, 0, 0, 0, 0, 0};

    parseArgs(argc, argv, &dataSig, &cmdSig, &binPath, &txtPath, &flags.useBin);

    openFiles(&txtFd, txtPath, &binFd, binPath, flags.useBin);

    ignoreSignal(dataSig);  // instead of blocking, for signal not to be queued
    registerSignalHandler(cmdSig, cmdSignalNotifier);

    runRecorder(dataSig, cmdSig, txtFd, binFd, &flags);

    // No need to close files here because the only way to exit is to terminate

    return 0;
}

void openFiles(int* txtFd, char* txtPath, int* binFd, char* binPath, int useBinFlag)
{
    *txtFd = openFile(txtPath);
    if(useBinFlag)
        *binFd = openFile(binPath);
}

void runRecorder(int dataSig, int cmdSig, int txtFd, int binFd, ProgramFlags* flags)
{
    struct timespec refPoint;

    while(1)
    {
        pause();

        if(cmdSigInfo.notified)
        {
            sigset_t backup;
            blockSignal(cmdSig, &backup);  // block handling of cmd signals (enter "critical section")

            executeCmd(txtFd, binFd, dataSig, cmdSig, &refPoint, flags);

            unblockSignal(cmdSig, &backup); // unblock handling of cmd signal (left "critical section")
        }

        if(dataSigInfo.notified)
        {
            sigset_t backup;
            blockSignal(dataSig, &backup);  // block handling of data signals (enter "critical section")

            saveData(txtFd, binFd, &refPoint, flags);

            unblockSignal(dataSig, &backup); // unblock handling of data signal (left "critical section")
        }
    }
}

void executeCmd(int txtFd, int binFd, int dataSig, int cmdSig, struct timespec* refPoint, ProgramFlags* flags)
{
    cmdSigInfo.notified = 0;

    decodeCmd(cmdSigInfo.data, flags);

    if(flags->stopped)
    {
        ignoreSignal(dataSig);  // instead of blocking, for signal not be queued
    }
    else if(flags->sendInfo)
    {
        flags->sendInfo = 0;
        int info = codeFlags(flags);
        sendInfo(cmdSigInfo.senderPid, cmdSig, info);
    }
    else    // Start command
        executeStartCmd(txtFd, binFd, dataSig, refPoint, flags);
}

void executeStartCmd(int txtFd, int binFd, int dataSig, struct timespec* refPoint, ProgramFlags* flags)
{
    if(isSignalIgnored(dataSig)) // reset handler function (start handling)
        registerSignalHandler(dataSig, dataSignalNotifier);

    if(flags->truncFiles)
    {
        flags->truncFiles = 0;
        truncFile(txtFd);
        if(flags->useBin)
            truncFile(binFd);
    }

    // update refpoint or create one if old ref is demanded and it not exists:
    if(flags->updateRefPoint ||(flags->useRefPoint && !flags->hasRefPoint))
    {
        flags->updateRefPoint = 0;
        getTimestamp(CLOCK_MONOTONIC, refPoint);
    }
}

void saveData(int txtFd, int binFd, struct timespec* refPoint, ProgramFlags* flags)
{
    dataSigInfo.notified = 0;
    float value;
    memcpy(&value, (void*)&dataSigInfo.data, sizeof(float));    // way to interpret non float 4-byte as float

    pid_t pid = -1;

    if(flags->usePid)
        pid = dataSigInfo.senderPid;

    struct timespec timestamp;

    if(flags->useRefPoint)
    {
        struct timespec temp;
        getTimestamp(CLOCK_MONOTONIC, &temp);
        getTimestampDiff(&timestamp, refPoint, &temp);
    }
    else
        getTimestamp(CLOCK_REALTIME, &timestamp);

    writeRecordTxt(txtFd, &timestamp, value, pid, flags);

    if(flags->useBin)
        writeRecordBin(binFd, &timestamp, &value, &pid);
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