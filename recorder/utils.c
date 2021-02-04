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

void writeTimestampGlobal(int fd)
{
    struct timespec ts;

    if(clock_gettime(CLOCK_REALTIME, &ts))
        errExit("writeTimestampGlobal: Unable to get clock value");

    struct tm readable;

    if(!localtime_r(&(ts.tv_sec), &readable))
        errExit("writeTimestampGlobal: Unable to get human readable time values");

    char buf[MAX_TIMESTAMP_LEN] = {0};

    long milliseconds = ts.tv_nsec / 1000000;

    sprintf(buf, "%d:%d:%d.%ld",
            readable.tm_hour, readable.tm_min, readable.tm_sec, milliseconds);

    if(write(fd, buf, strlen(buf)) == -1)
        errExit("writeTimestampGlobal: Unable to write timestamp to file");
}

void writeTimestampLocal(int fd, struct timespec* refPoint)
{
    struct timespec diff;
    struct timespec curr;

    if(clock_gettime(CLOCK_MONOTONIC, &curr))
        errExit("writeTimestampLocal: Unable to get clock value");

    getTimestampDiff(&diff, refPoint, &curr);

    time_t hours = diff.tv_sec / 3600;
    time_t mins = (diff.tv_sec % 3600) / 60;
    time_t sec = ((diff.tv_sec % 3600) % 60) / 60;
    long milliseconds = diff.tv_nsec / 1000000;

    char buf[MAX_TIMESTAMP_LEN] = {0};

    sprintf(buf, "%ld:%ld:%ld.%ld",
            hours, mins, sec, milliseconds);

    if(write(fd, buf, strlen(buf)) == -1)
        errExit("writeTimestampLocal: Unable to write timestamp to file");
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