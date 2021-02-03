#include <stdio.h>
#include <time.h>
#include <zconf.h>  // TODO: delete
#include <memory.h> // TODO: delete

#include "utils.h"
#include "parse.h"
#include "udp.h"

volatile uint8_t alrm = 0;
volatile uint8_t io = 0;

void alrmHandler(int signo);
void ioHandler(int signo);

int main(int argc, char* argv[])
{
    if(argc != 2)
        usageExit(argv[0], "<short int>");

    short port = strToPort(argv[1]);
    int sockfd = initSocket(port);

    SimParams  params;
    struct sigevent sevp;
    initParamsDefaults(&params, &sevp);
    SimFlags flags;
    initFlagsDefaults(&flags);

    registerSignalHandler(SIGALRM, alrmHandler);
    registerSignalHandler(SIGIO, ioHandler);

    //timer_t timerId;
    //createTimer(&sevp, &timerId);
    armTimer(params.timerId, params.probe);

    char dgramBuf[MAX_DGRAM_LEN] = {0};

    double sinValue = 0;
    float timeRemaining = params.period;
    double refPoint = getTimestampSec();

    while(timeRemaining > 0 || flags.stopped)
    {
        flags.suspended = (uint8_t)(flags.stopped == 2 || flags.pidErr || flags.rtOutOfRange);
        pause();

        if(io)
        {
            io = 0;

            struct sockaddr_in sender = {0};
            socklen_t senderLen = sizeof(sender);

            if(recvfrom(sockfd, &dgramBuf, sizeof(dgramBuf), MSG_DONTWAIT, (struct sockaddr*)&sender, &senderLen) != -1)
            {
                interpretDatagram(dgramBuf, &params, &flags);
                memset(dgramBuf, 0, sizeof(dgramBuf));

                if(flags.report)
                {
                    createReport(dgramBuf, &params, &flags);
                    if(sendto(sockfd, &dgramBuf, sizeof(dgramBuf), 0, (struct sockaddr*)&sender, senderLen) == -1)
                        errExit("Unable to send report");

                    flags.report = 0;
                    memset(dgramBuf, 0, sizeof(dgramBuf));
                }
            }
            else
                errExit("Unable to receive datagram");
        }

        if(alrm)
        {
            alrm = 0;

            if(!flags.suspended)
            {
                sinValue = calcSinusoide(&params, getTimestampSec() - refPoint);
                sendRtSignal(&params, &flags, &sinValue);

                timeRemaining -= params.probe;
            }
        }

        if(flags.reset)
        {
            refPoint = getTimestampSec();   // reset reference point
            armTimer(params.timerId, params.probe);    // reset timer
        }
    }

    /*while(1)
    {
        //pause()
        //if(io)
            //io = 0;
            //recvfrom
            //interpret
            //if report
                //sendto
                //report = 0
         //if(alrm)
            //send realtime sinusoide val
            //update pid flag
         //if(restart)
            //get ref point
            //restart alarm

        struct sockaddr_in sender = {0};
        socklen_t senderLen = sizeof(sender);

        errno = 0;
        ssize_t received = recvfrom(sockfd, &buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr*)&sender, &senderLen);
        if(received != -1)
        {
            interpretDatagram(buf, &params, &flags);
            memset(buf, 0, sizeof(buf));

            if(flags.report)
            {
                createReport(buf, &params, &flags);
                if(sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr*)&sender, senderLen) == -1)
                    errExit("Unale to send report");

                flags.report = 0;
                memset(buf, 0, sizeof(buf));
            }
        }
        else if(errno == EAGAIN)
        {
            sleep(5);
            //printf("\t>wake up!\n");
        }
        else
            errExit("Unable to receive datagram");
    }*/

    timer_delete(params.timerId);
    shutdown(sockfd, SHUT_RDWR);

    return 0;
}

void alrmHandler(int signo)
{
    alrm = 1;
}

void ioHandler(int signo)
{
    io = 1;
}