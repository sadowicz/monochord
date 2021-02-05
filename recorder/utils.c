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

void sendInfo(pid_t pid, int signal, int info)
{
    union sigval sval;
    sval.sival_int = info;

    if(sigqueue(pid, signal, sval))
        errExit("sendInfo: Unable to send signal");
}

int codeFlags(ProgramFlags* flags)
{
    int code = 0;

    if(!flags->stopped)
        code |= 0x1;    // 0001
    if(flags->useRefPoint)
        code |= 0x2;    // 0010
    if(flags->usePid)
        code |= 0x4;    // 0100
    if(flags->useBin)
        code |= 0x8;    // 1000

    return code;
}

void decodeCmd(int cmdData, ProgramFlags* flags)
{
    switch(cmdData)
    {
        case 0:
            flags->stopped = 1;
            break;
        case 255:
            flags->sendInfo = 1;
            break;
        default:
        {
            flags->stopped = 0;

            if(cmdData - 8 >= 0)
            {
                flags->truncFiles = 1;
                cmdData -= 8;
            }

            if(cmdData - 4 >= 0)
            {
                flags->usePid = 1;
                cmdData -= 4;
            }

            if(cmdData == 3)
                flags->useRefPoint = 1;
            else if(cmdData == 2)
            {
                flags->useRefPoint = 1;
                flags->updateRefPoint = 1;
            }
            else if(cmdData < 1)
                errExit("decodeCmd: Invalid command value");

            break;
        }
    }
}

int openFile(char* path)
{
    int fd = STDOUT_FILENO;

    if(strcmp(path, "-"))
    {
        int flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        fd = open(path, flags, mode);
        if(fd == -1)
            errExit("openFile: Unable to open/create file");
    }

    return fd;
}

void truncFile(int fd)
{
    if(fd != STDOUT_FILENO)
    {
        if(ftruncate(fd, 0))
            errExit("truncFile: Unable to truncate file length.");

        if(lseek(fd, 0, SEEK_SET) == -1)
            errExit("truncFile: Unable to set file offset at file beginning.");
    }
}

void writeRecordTxt(int fd, struct timespec* timestamp, float data, pid_t pid, ProgramFlags* flags)
{
    char* buffer = calloc(MAX_RECORD_LEN, sizeof(char));
    if(!buffer)
        errExit("writeRecordTxt: Unable to allocate write buffer.");

    if(flags->useRefPoint)
        getStrTimestampGlobal(timestamp, buffer);
    else
        getStrTimestampLocal(timestamp, buffer);

    if(sprintf(buffer + strlen(buffer), " %f", data) <= 0 )
        errExit("writeRecordTxt: Unable to write float data into buffer.");

    if(flags->usePid)
    {
        if(pid == -1)
            errExit("writeRecordTxt: No pid given");

        if(sprintf(buffer + strlen(buffer), " %d", pid) <= 0 )
            errExit("writeRecordTxt: Unable to write pid into buffer.");
    }

    if(sprintf(buffer + strlen(buffer), "\n") <= 0 )
        errExit("writeRecordTxt: Unable to write record separator into buffer.");

    if(write(fd, buffer, strlen(buffer)) == -1)
        errExit("writeRecordTxt: Unable to write buffer into text file.");

    free(buffer);
}

void writeRecordBin(int fd, struct timespec* timestamp, float* data, pid_t* pid)
{
    if(write(fd, timestamp, sizeof(struct timespec)) == -1)
        errExit("writeRecordBin: Unable to write timestamp.");

    if(write(fd, data, sizeof(float)) == -1)
        errExit("writeRecordBin: Unable to write data.");

    if(write(fd, pid, sizeof(pid_t)) == -1)
        errExit("writeRecordBin: Unable to write pid.");
}

void getStrTimestampGlobal(struct timespec* timestamp, char* buffer)
{
    struct tm readable;

    if(!localtime_r(&(timestamp->tv_sec), &readable))
        errExit("getStrTimestampGlobal: Unable to get human readable time values");

    long milliseconds = timestamp->tv_nsec / 1000000;

    // dd/mm/yyyy HH:MM:SS.MS
    int written = sprintf(buffer, "%d/%d/%d %d:%d:%d.%ld",
            readable.tm_mday, readable.tm_mon + 1, readable.tm_year + 1900,
            readable.tm_hour, readable.tm_min, readable.tm_sec, milliseconds);

    if(written <= 0)
        errExit("getStrTimestampGlobal: Unable to write timestamp to buffer");
}

void getStrTimestampLocal(struct timespec* timestamp, char* buffer)
{
    time_t hours = timestamp->tv_sec / 3600;
    time_t mins = (timestamp->tv_sec % 3600) / 60;
    time_t sec = ((timestamp->tv_sec % 3600) % 60) / 60;
    long milliseconds = timestamp->tv_nsec / 1000000;

    // cH:MM:SS.MS
    int written = sprintf(buffer, "%ld:%ld:%ld.%ld",
            hours, mins, sec, milliseconds);

    if(written <= 0)
        errExit("getStrTimestampLocal: Unable to write timestamp to buffer");
}

void getTimestamp(clockid_t clockType, struct timespec* timestamp)
{
    if(clock_gettime(clockType, timestamp))
        errExit("getTimestamp: Unable to get timestamp for given clock");
}

void getTimestampDiff(struct timespec* res, struct timespec* earlier, struct timespec* later)
{
    res->tv_sec = later->tv_sec - earlier->tv_sec;

    if(later->tv_nsec < earlier->tv_nsec)
    {
        res->tv_sec--;
        res->tv_nsec = 1000000000 - earlier->tv_nsec + later->tv_nsec;
    }
}