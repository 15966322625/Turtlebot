#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#include "my_err.h"
#include "IMU_UDP.h"

imu_udp::imu_udp(uint16_t port, int no)
{
    this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &this->reuse_addr, sizeof(this->reuse_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(port);
    this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(this->socket_fd, (struct sockaddr*)&this->server_addr, sizeof(this->server_addr)) < 0)
    {
        print_err("bind_err\r\n", __LINE__, errno);
    }
    this->imu_no = no;
    this->connect_flag = false;
    this->new_data = false;
}

int imu_udp::data_conv(char* buff, float* data_conved)
{
    char temp[30] = {0};
    int start = 0;
    int count = -1;
    int i = 0;
    int length  = strlen(buff);
    for (i = 0; i < length + 1; i++)
    {
        if (i == DATA_START || buff[i] == ',' || buff[i] == '\0')
        {
            if (start != 0)
            {
                temp[i-start-1] = '\0';
                count++;
                data_conved[count] = strtof(temp, NULL);
            }
            start = i;
        }
        if (start != 0)
        {
            temp[i-start] = buff[i+1];
        }
    }

    return count + 1;
}

void* imu_udp::imu_wifi_thread(void* arg)
{
    printf("进入imu 线程\n");
    extern pthread_mutex_t mutex_imu_data[4];

    imu_udp* p_imu = (imu_udp*)arg;
    int addr_size = sizeof(p_imu->server_addr);
    int recv_len;
    char buff[300];
    // int count = 0;
    long time = -1;
    float data_temp[17];

    struct timeval start;
    struct timeval end;
    

    while(1)
    {
        if (p_imu->proc_switch == 0)
        {
            printf("imu_%d=======exit\r\n", p_imu->imu_no);
            pthread_exit(0);
        }
        bzero(&(p_imu->client_addr), sizeof(p_imu->client_addr));
        memset(buff, 0, 300);
        recv_len = recvfrom(p_imu->socket_fd, buff, 300, 0, (struct sockaddr*)&(p_imu->client_addr), (socklen_t*)&addr_size);
        {
            if(!p_imu->connect_flag)
            {
                printf(" %dth imu connect success\n ",p_imu->imu_no);
                p_imu->connect_flag = true;
            }
        }
        if (recv_len <= 0)
        {
            print_err("recv err\r\n", __LINE__, errno);
        }
        //count = p_imu->data_conv(buff, data_temp);
        p_imu->data_conv(buff, data_temp);
        gettimeofday(&end, NULL);
        if (time == -1)
        {
            start.tv_sec = end.tv_sec;
            start.tv_usec = end.tv_usec;
            time = 0;
        }
        else
        {
            time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            start.tv_sec = end.tv_sec;
            start.tv_usec = end.tv_usec;
        }
        pthread_mutex_lock(&p_imu->mutex);
        memcpy(p_imu->data_temp, data_temp, 17*sizeof(float));
        p_imu->time = time;
        p_imu->new_data = true;
        pthread_mutex_unlock(&p_imu->mutex);
    }
}
