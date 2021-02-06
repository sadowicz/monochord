#include <stdio.h>

#include "utils.h"
#include "parse.h"
#include "udp.h"

volatile uint8_t alrm = 0;
volatile uint8_t io = 0;

void initSim(short port, int* sockfd, SimParams* params, SimFlags* flags, char** dgramBuf);

void performSim(int sockfd, SimParams* params, SimFlags* flags, char* dgramBuf);
void processSimIO(int sockfd, char* dgramBuf, SimParams* params, SimFlags* flags);

void cleanUpSim(int sockfd, SimParams* params, char** dgramBuf);

void alrmNotifier(int signo);
void ioNotifier(int signo);

int main(int argc, char* argv[])
{
    if(argc != 2)
        usageExit(argv[0], "<short int>");

    short port = strToPort(argv[1]);
    int sockfd;
    SimParams params;
    SimFlags flags;
    char* dgramBuf;

    initSim(port, &sockfd, &params, &flags, &dgramBuf);
    performSim(sockfd, &params, &flags, dgramBuf);
    cleanUpSim(sockfd, &params, &dgramBuf);

    return 0;
}

void initSim(short port, int* sockfd, SimParams* params, SimFlags* flags, char** dgramBuf)
{
    *sockfd = initSocket(port);

    initParamsDefaults(params);
    initFlagsDefaults(flags);

    registerSignalHandler(SIGALRM, alrmNotifier);
    registerSignalHandler(SIGIO, ioNotifier);

    armTimer(params->timerId, params->probe);

    *dgramBuf = (char*)calloc(MAX_DGRAM_LEN, sizeof(char));
    if(!*dgramBuf)
        errExit("initSim: Unable to allocate diagram buffer");
}

void performSim(int sockfd, SimParams* params, SimFlags* flags, char* dgramBuf)
{
    float sinValue = 0;
    double refPoint = getTimestampSec();

    while(params->timeRemaining > 0 || flags->stopped)
    {
        flags->suspended = (uint8_t)(flags->stopped == 2 || flags->pidErr || flags->rtOutOfRange);
        pause();

        if(io)
        {
            io = 0;
            processSimIO(sockfd, dgramBuf, params, flags);
        }

        if(alrm)
        {
            alrm = 0;

            if(!flags->suspended)
            {
                sinValue = calcSinusoide(params, getTimestampSec() - refPoint);
                sendRtSignal(params, flags, sinValue);

                params->timeRemaining -= params->probe;
            }
        }

        if(flags->reset)
        {
            refPoint = getTimestampSec();   // reset reference point
            armTimer(params->timerId, params->probe);    // reset timer
        }
    }
}

void processSimIO(int sockfd, char* dgramBuf, SimParams* params, SimFlags* flags)
{
    struct sockaddr_in sender = {0};
    socklen_t senderLen = sizeof(sender);

    if(recvfrom(sockfd, dgramBuf, MAX_DGRAM_LEN, MSG_DONTWAIT, (struct sockaddr*)&sender, &senderLen) != -1)
    {
        interpretDatagram(dgramBuf, params, flags);
        memset(dgramBuf, 0, MAX_DGRAM_LEN);

        if(flags->report)
        {
            createReport(dgramBuf, params, flags);
            if(sendto(sockfd, dgramBuf, MAX_DGRAM_LEN, 0, (struct sockaddr*)&sender, senderLen) == -1)
                errExit("Unable to send report");

            flags->report = 0;
            memset(dgramBuf, 0, MAX_DGRAM_LEN);
        }
    }
    else
        errExit("handleIO: Unable to receive datagram");
}

void cleanUpSim(int sockfd, SimParams* params, char** dgramBuf)
{
    free(*dgramBuf);
    timer_delete(params->timerId);
    shutdown(sockfd, SHUT_RDWR);
}

void alrmNotifier(int signo)
{
    alrm = 1;
}

void ioNotifier(int signo)
{
    io = 1;
}
