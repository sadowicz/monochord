#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 200809L

#define PI 3.14159265359

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>

typedef struct SimParams {
    float amp;      // amplitude
    float freq;     // frequency [Hz]
    float probe;    // probe frequency [s]
    float period;   // probing (simulation) time [s]
    // 0 -infinite  /   <0 - pause probing
    pid_t pid;
    int rt;

    float timeRemaining;
    timer_t timerId;
}SimParams;

typedef struct SimFlags {
    uint8_t reset;
    uint8_t report;
    uint8_t suspended;
    uint8_t stopped;    // 0 : default , 1 : non-stop , 2 : stopped
    uint8_t pidErr;     // 0 : can send signal , 1 : cannot send , 2 : not-exists
    uint8_t rtOutOfRange;
}SimFlags;

void errExit(const char* msg);
void usageExit(const char* programName, const char* msg);

void initParamsDefaults(SimParams* params);
void  initFlagsDefaults(SimFlags* flags);

void registerSignalHandler(int signal, void(*handler)(int));
void sendRtSignal(SimParams* params, SimFlags* flags, double* value);

void createTimer(timer_t* timerId);
void armTimer(timer_t timerId, float interval);

double getTimestampSec();

double calcSinusoide(SimParams* params, double time);

#endif