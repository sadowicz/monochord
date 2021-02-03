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

    createTimer(&params->timerId);
    params->timeRemaining = params->period;
}

void initFlagsDefaults(SimFlags* flags)
{
    flags->report = 0;
    flags->reset = 0;
    flags->stopped = 2;
    flags->suspended = 0;
    flags->pidErr = 0;
    flags->rtOutOfRange = 1;
}

void registerSignalHandler(int signal, void(*handler)(int))
{
    struct sigaction sa;

    if(sigemptyset(&sa.sa_mask))
        errExit("registerSignalHandler: Unable to empty sa_mask");

    if(sigaddset(&sa.sa_mask, signal))
        errExit("registerSignalHandler: Unable to add signal to sa_mask");

    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if(sigaction(signal, &sa, NULL))
        errExit("registerSignalHandler: Unable to register signal handler");
}

void sendRtSignal(SimParams* params, SimFlags* flags, double* value)
{
    union sigval val;
    val.sival_ptr = (void*)value;

    errno = 0;

    if(sigqueue(params->pid, params->rt, val))
    {
        if(errno == ESRCH)
            flags->pidErr = 2;
        else
            flags->pidErr = 1;
    }
    else
        flags->pidErr = 0;
}

void createTimer(timer_t* timerId)
{
    struct sigevent sevp;

    sevp.sigev_notify = SIGEV_SIGNAL;
    sevp.sigev_signo = SIGALRM;

    if(timer_create(CLOCK_REALTIME, &sevp, timerId))
        errExit("createTimer: Unable to create timer.");

}

void armTimer(timer_t timerId, float interval)
{
    time_t sec = (time_t)(interval * 100000000) / 1000000000;
    long nsec = (long)(interval * 100000000) % 1000000000;

    struct timespec ts = { .tv_sec = sec, .tv_nsec = nsec };
    struct itimerspec value = {.it_interval = ts, .it_value = ts};

    if(timer_settime(timerId, 0, &value, NULL))
        errExit("armTimer: Unable to arm timer.");
}

double getTimestampSec()
{
    struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts))
        errExit("getTimestampSec: Unable to get clock time");

    return ts.tv_sec + (double)ts.tv_nsec / 1000000000;
}

double calcSinusoide(SimParams* params, double time)
{
    return params->amp * sin(2 * PI * params->freq * time);
}