#ifndef UDP_H
#define UDP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"

int initSocket(short port);
void registerSocket(int sockfd, short port);

#endif