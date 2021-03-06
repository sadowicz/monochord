#include "udp.h"

int initSocket(short port)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
        errExit("initSocket: Unable to create socket file descriptor");

    registerSocket(sockfd, port);
    setSockfdForSignalIO(sockfd);

    return sockfd;
}

void registerSocket(int sockfd, short port)
{
    struct sockaddr_in sAddr = {
            .sin_family = AF_INET,
            .sin_port = htons((in_port_t)port),
            .sin_addr.s_addr = htonl(INADDR_LOOPBACK)
    };

    if(bind(sockfd, (struct sockaddr*)&sAddr, sizeof(sAddr)))
        errExit("registerSocket: Unable to bind socket");
}

void setSockfdForSignalIO(int sockfd)
{
    if(fcntl(sockfd, F_SETOWN, getpid()) == -1)
        errExit("setSockfdForSignalIO: Unable to set calling process as fd owner");

    int flags = fcntl(sockfd, F_GETFL);
    if(flags == -1)
        errExit("setSockfdForSignalIO: Unable to get sockfd flags");

    if(fcntl(sockfd, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1)
        errExit("setSockfdForSignalIO: Unable to set sockfd flags");
}