#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>

#include "utils.h"
#include "parse.h"
#include "udp.h"

typedef struct SimParams {
    float amp;      // amplitude
    float freq;     // frequency [Hz]
    float probe;    // probe frequency [s]
    float period;   // probing (simulation) time [s]
                    // 0 -infinite  /   <0 - pause probing
    pid_t pid;
    int rt;
}SimParams;

void initParamsDefaults(SimParams* params);
double getTimestampSec();

int main(int argc, char* argv[])
{
    if(argc != 2)
        usageExit(argv[0], "<short int>");

    short port = strToPort(argv[1]);
    int sockfd = initSocket(port);

    SimParams  params;
    initParamsDefaults(&params);

    double refPoint = getTimestampSec();

    

    // TODO: close sockfd

    return 0;
}

void initParamsDefaults(SimParams* params)
{
    params->amp = 1.f;
    params->freq = .25f;
    params->probe = 1.f;
    params->period = -1.f;
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