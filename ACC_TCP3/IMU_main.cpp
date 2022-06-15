#include "IMU_main.h"

void imu_thread::imu_thread_init(imu_udp** imu_list, int count)
{
    for (int i = 0; i < count; i++)
    {
        this->imu_list[i] = imu_list[i];
    }
    this->imu_count = count;
    for (int i = 0; i < this->imu_count; i++)
    {
        pthread_create(&(this->tid_list[i]), NULL, this->imu_list[i]->imu_wifi_thread, this->imu_list[i]);
    }
}

void imu_thread::get_data(float data[][12],bool& dataIsNew)
{
    for (int i = 0; i < this->imu_count; i++)
    {
        pthread_mutex_lock(&(this->imu_list[i]->mutex));
        data[i][0]  = this->imu_list[i]->data_temp[0];
        data[i][1]  = this->imu_list[i]->data_temp[1];
        data[i][2]  = this->imu_list[i]->data_temp[2];
        data[i][3]  = this->imu_list[i]->data_temp[3];
        data[i][4]  = this->imu_list[i]->data_temp[4];
        data[i][5]  = this->imu_list[i]->data_temp[5];
        data[i][10] = this->imu_list[i]->data_temp[7];
        data[i][11] = this->imu_list[i]->data_temp[6];
        dataIsNew = this->imu_list[i]->new_data;
        this->imu_list[i]->new_data = false;
        pthread_mutex_unlock(&(this->imu_list[i]->mutex));
    }
}

void imu_thread::imu_thread_exit()
{
    for (int i = 0; i < this->imu_count; i++)
    {
        this->imu_list[i]->proc_switch = 0;
    }
    for (int i = 0; i < this->imu_count; i++)
    {
        pthread_join(this->tid_list[i], NULL);
    }
    for (int i = 0; i < this->imu_count; i++)
    {
        pthread_mutex_destroy(&(this->imu_list[i]->mutex));
    }
}
