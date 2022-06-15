#ifndef __IMU_MAIN_H
#define __IMU_MAIN_H
#include "IMU_UDP.h"
#include <pthread.h>

class imu_thread
{
private:
    imu_udp* imu_list[8];
    pthread_t  tid_list[8];
    int imu_count;
public:
    void imu_thread_init(imu_udp** imu_list, int count);
    void get_data(float data[][12],bool& dataIsNew);
    void imu_thread_exit();
};

#endif 