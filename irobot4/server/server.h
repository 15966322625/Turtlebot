#ifndef __SERVER_H
#define __SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

//SERVER
#define SPORT 2333
//#define SIP inet_addr(SIP)
#define SIP htonl(INADDR_ANY)

class Server
{
public:
    void init();
    void tcp_send(char *lable, char len);
    void tcp_close();
public:
    static void* server_main(void* arg);
private:
    int sockfd;
    int cfd;
};

#endif