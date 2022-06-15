#include "global.h"

GLB *p_shm_GLB; //进程id和状态共享内存
SHM_TCP_t *p_shm_TCP;//tcp传输共享内存
SHM_IMU *p_shm_imu;


//信号量描述符

int semid_imu; //暂存imu数据
int semid_data; //保存数据包
int semid_ACT;	//保存数据窗
int semid_TCP;	//保存tcp数据包
int semid_glbs;	//保存全局进程id和状态
int semid_timer;	//保存时间窗
int semid_foot;


//共享内存描述符

int shmid_data;
int shmid_imu;
int shmid_ACT;
int shmid_TCP;
int shmid_glbs;
int shmid_timer;
int shmid_foot;