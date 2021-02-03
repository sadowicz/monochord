#ifndef UDP_H
#define UDP_H

#define MAX_DGRAM_LEN 512

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "utils.h"

int initSocket(short port);
void registerSocket(int sockfd, short port);
void setSockfdForSignalIO(int sockfd);

#endif