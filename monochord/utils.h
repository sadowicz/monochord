#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdint.h>

typedef struct SimParams {
    float amp;      // amplitude
    float freq;     // frequency [Hz]
    float probe;    // probe frequency [s]
    float period;   // probing (simulation) time [s]
    // 0 -infinite  /   <0 - pause probing
    pid_t pid;
    int rt;
}SimParams;

typedef struct SimFlags {
    uint8_t reset;
    uint8_t report;
    uint8_t suspended;
    uint8_t stopped;    // 0 : default , 1 : non-stop , 2 : stopped
    uint8_t pidNotExist;
    uint8_t rtOutOfRange;
}SimFlags;

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void initParamsDefaults(SimParams* params);
void  initFlagsDefaults(SimFlags* flags);

double getTimestampSec();

void createReport(char* report, SimParams* params, SimFlags* flags);

#endif