#ifndef main_IMU_data_
#define main_IMU_data_
#include "configure.h"


void main_data();




struct IMU_Data
{
	float ACC_x[50];
	float ACC_y[50];
	float ACC_z[50];
	float GYRO_x[50];
	float GYRO_y[50];
	float GYRO_z[50];
	float Q_0[50];
	float Q_1[50];
	float Q_2[50];
	float Q_3[50];
	float Pitch[50];
	float Yaw[50];
};


struct SHM_IMU_data
{
	IMU_Data Data_IMU[IMU_CH] = {0}; //记录imu数据
	int count=0; //记录有多少个imu数据
	int T_now=0; // 记录最后一个imu采集数据的时间
	SHM_IMU_data()
	{
		count = 0;
		T_now = 0;
	}
};


#endif