#ifndef __IMU_UDP_H
#define __IMU_UDP_H

#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

#define DATA_START  13

class imu_udp
{
public:
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int socket_fd;
    int state = 0;
    int imu_no;
    int proc_switch = 1;
public:
    imu_udp(uint16_t port, int no);
    static void* imu_wifi_thread(void* arg);
    float data_temp[17];
    long time;
    pthread_mutex_t mutex;
bool connect_flag;
 bool new_data;
private:
    int data_conv(char* buff, float* data_conved);
    int reuse_addr;
//    bool connect_flag;
};

#endif /* __IMU_UDP_H */
