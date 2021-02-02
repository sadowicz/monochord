#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <zconf.h>  // TODO: delete
#include <memory.h> // TODO: delete

#include "utils.h"
#include "parse.h"
#include "udp.h"

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
        errno = 0;
        ssize_t received = recv(sockfd, &buf, sizeof(buf), MSG_DONTWAIT);
        if(received != -1)
        {
            interpretDatagram(buf, &params, &flags);
            memset(buf, 0, sizeof(buf));
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