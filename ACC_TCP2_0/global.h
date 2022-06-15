#pragma once
#include <sys/time.h>

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "configure.h"


struct PID_t
{
	pid_t PID_MAIN_TCP; 
	pid_t PID_IMU; 
	pid_t PID_DATA;
	pid_t PID_MAIN;
};

struct proc_status_t
{
	int proc_status_proc;
	int en_dsp;
	int en_rcg;
	int en_tcp;
	int ACT_OVER;
	int proc_TCP_status;
	int proc_ACT_status;   
	int T_status;   
};

struct GLB 
{
	struct PID_t PID;
	struct proc_status_t proc_status;
};

typedef struct {
	int Status;
	double DurTime;
}SHM_TCP_t;


struct SHM_IMU
{
	bool flag = false;
	float data[IMU_CH][12] = {0};
};


extern GLB *p_shm_GLB; //进程id和状态共享内存
extern SHM_TCP_t *p_shm_TCP;//tcp传输共享内存
extern SHM_IMU *p_shm_imu;


//信号量描述符

extern int semid_imu; //暂存imu数据
extern int semid_data; //保存数据包
extern int semid_ACT;	//保存数据窗
extern int semid_TCP;	//保存tcp数据包
extern int semid_glbs;	//保存全局进程id和状态
extern int semid_timer;	//保存时间窗
extern int semid_foot;


//共享内存描述符

extern int shmid_data;
extern int shmid_imu;
extern int shmid_ACT;
extern int shmid_TCP;
extern int shmid_glbs;
extern int shmid_timer;
extern int shmid_foot;


