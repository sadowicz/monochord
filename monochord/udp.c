#include "udp.h"

int initSocket(short port)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
        errExit("initSocket: Unable to create socket file descriptor");

    registerSocket(sockfd, port);

    return sockfd;
}

void registerSocket(int sockfd, short port)
{
    struct sockaddr_in sAddr = {
            .sin_family = AF_INET,
            .sin_port = htons((in_port_t)port),
            .sin_addr.s_addr = htonl(INADDR_ANY)    // htonl probably not necessary since INADDR_ANY is 0,
                                                    // but placed here for cleaner code
    };

    if(bind(sockfd, (struct sockaddr*)&sAddr, sizeof(sAddr)))
        errExit("registerSocket: Unable to bind socket");
}