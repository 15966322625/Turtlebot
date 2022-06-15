#include "server.h"

void Server::init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket fail\r\n");
        exit(-1);
    }
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;    //使用ipv4
    saddr.sin_port = htons(SPORT); //设定端口,htons用于端序转换
    saddr.sin_addr.s_addr = SIP;   //设定ip，inet_addr用于讲字符串ip转成32位无符号整形
    int opt = 1, ret = -1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret != 0)
    {
        printf("setsockopt failed\r\n");
        exit(-1);
    }
    ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        printf("bind fail\r\n");
        exit(-1);
    }
    ret = listen(sockfd, 3);
    if (ret == -1)
    {
        printf("listen fail\r\n");
        exit(-1);
    }
}

void Server::tcp_send(char *lable, char len)
{
    int ret = send(cfd, (void *)lable, len, 0);
    if (ret == -1)
    {
        printf("send error\r\n");
        exit(-1);
    }
}

void Server::tcp_close()
{
    close(sockfd);
    close(cfd);
}

void *Server::server_main(void *arg)
{
    struct sockaddr_in clnaddr = {0}; //存放客户的ip和端口
    char data[10];
    int ret;
    int clnaddr_size = sizeof(clnaddr);

    extern int pipe_fd[2];

    Server* p_server = (Server*)arg;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    p_server->init();
    printf("\n========Waiting for connection========\n");
    p_server->cfd = accept(p_server->sockfd, (struct sockaddr *)&clnaddr, (socklen_t *)&clnaddr_size); //创建监听
    if (p_server->cfd == -1)
    {
        printf("accept fail\r\n");
        exit(-1);
    }
    printf("========Connected========\n");
    printf("clint_port=%d\nclint_ip=%s\n", ntohs(clnaddr.sin_port), inet_ntoa(clnaddr.sin_addr));
    while (1)
    {
        bzero(data, sizeof(data));
        ret = recv(p_server->cfd, data, sizeof(data), 0);
        if (ret == -1)
        {
            printf("recv fail\r\n");
            exit(-1);
        }
        if (ret == 0)
        {
            printf("======client closed======\n");
            close(p_server->sockfd);
            close(p_server->cfd);
            kill(getpid(), SIGINT);
        }
        if (ret > 0)
        {
            char pipe_buf;
            pipe_buf = data[0] - '0';
            write(pipe_fd[1], &pipe_buf, 1);
            memset(data, 0, 10);
        }
    }
}