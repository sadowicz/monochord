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

void initParamsDefaults(SimParams* params)
{
    params->amp = 1.0f;
    params->freq = 0.25f;
    params->probe = 1.0f;
    params->period = -1.0f;
    params->pid = 1;
    params->rt = 0;
}

double getTimestampSec()
{
    struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts))
        errExit("getTimestampSec: Unable to get clock time");

    return ts.tv_sec + (double)ts.tv_nsec / 1000000000;
}

void createReport(char* report, SimParams* params, SimFlags* flags)
{
    int printed = sprintf(report, "amp %f\nfreq %f\nprobe %f\nperiod %f",
            params->amp, params->freq, params->probe, params->period);
    if(printed == -1)
        errExit("createReport: Unable to print into report buffer");

    int pr = 0;

    if(params->period <= 0)
    {
        pr = sprintf(report + printed, (flags->stopped > 1) ? " stopped" : " non-stop");
        if(pr == -1)
            errExit("Unable to print stopped info into report buffer");

        printed += pr;
    }

    if(params->period >= 0 && flags->suspended)
    {
        pr = sprintf(report + printed, " suspended");
        if(pr == -1)
            errExit("Unable to print suspended info into report buffer");

        printed += pr;
    }

    pr = sprintf(report + printed, "\npid %d%srt %d%s",
            params->pid, (flags->pidNotExist)? " non-exists\n" : " exists\n",
            params->rt, (flags->rtOutOfRange)? " out-of-range\n" : " in-range");
    if(pr == -1)
        errExit("Unable to print pid into report buffer");
}