#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct SimParams {
    float amp;      // amplitude
    float freq;     // frequency [Hz]
    float probe;    // probe frequency [s]
    float period;   // probing (simulation) time [s]
    // 0 -infinite  /   <0 - pause probing
    pid_t pid;
    int rt;
}SimParams;

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void initParamsDefaults(SimParams* params);
double getTimestampSec();

#endif