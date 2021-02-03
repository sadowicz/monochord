#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <zconf.h>  // TODO: delete
#include <memory.h> // TODO: delete

#include "utils.h"
#include "parse.h"
#include "udp.h"

uint8_t alrm = 0;
uint8_t io = 0;

int main(int argc, char* argv[])
{
    if(argc != 2)
        usageExit(argv[0], "<short int>");

    short port = strToPort(argv[1]);
    int sockfd = initSocket(port);

    SimParams  params;
    initParamsDefaults(&params);

    SimFlags flags;
    initFlagsDefaults(&flags);

    double refPoint = getTimestampSec();

    char buf[512] = {0};

    while(1)
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
    }

    // TODO: close sockfd

    return 0;
}