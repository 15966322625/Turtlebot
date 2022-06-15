#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>


#include "configure.h"
#include "global.h"
#include "sem.h"
#include "shm.h"
#include "main_data.h"
#include "main_IMU.h"
#include <cstring>
#include <iostream>


using namespace std;
static int proc_status = 1;
static int tcp_flag  = 0;



//PID
static pid_t pid_data;
static pid_t pid_main;
static pid_t pid_imu;




static void APP_exit()
{
	sem_P(semid_glbs);
	pid_data = p_shm_GLB->PID.PID_DATA;
	pid_imu = p_shm_GLB->PID.PID_IMU;
	sem_V(semid_glbs);


	kill(pid_data, 12);
	#ifndef online
	kill(pid_imu,12);
	#endif



	waitpid(pid_data, NULL, 0);
	#ifndef online
	waitpid(pid_imu, NULL, 0);
	#endif



	shm_remove(semid_glbs, shmid_glbs, p_shm_GLB);
	shm_remove(semid_imu,shmid_imu,p_shm_imu);

	
	printf("=============== MAIN EXIT! ==============\n");
	exit(0);
}

static void SigH(int sig)
{
	switch (sig) 
	{
	case SIGINT:		//�ⲿ������Ҫ������˳���
		proc_status = STATUS_OFF;
		break;
	default:
		break;
	}
}

static bool init_sig()
{
	signal(SIGINT, SigH);
	return true;
}
//初始化信号量
static void init_sem()
{
	//glb
	semid_glbs = sem_creat();
	sem_init(semid_glbs);
	// //tcp
	// semid_TCP = Sem_creat();
	// Sem_set(semid_TCP);
	//imu
	semid_imu = sem_creat();
	sem_init(semid_imu);
	
	
}
//初始化共享内存
static void init_shm()
{
	void *shm_tmp = NULL;
//*********************GLB*******************
//
//******************************************
	shmid_glbs = shm_creat(sizeof(GLB));
	shm_tmp = NULL;
	shm_tmp = shm_at(shmid_glbs);
	p_shm_GLB = (GLB *)shm_tmp;


// //*********************TCP*******************
// //
// //******************************************
// 	shmid_TCP = Shm_creat(sizeof(SHM_TCP_t));
// 	shm_tmp = NULL;
// 	shm_tmp = Shm_mat(shmid_TCP);
// 	p_shm_TCP = (SHM_TCP_t *)shm_tmp;



//*********************imu*******************
//
//******************************************
	shmid_imu = shm_creat(sizeof(SHM_IMU));
	shm_tmp = NULL;
	shm_tmp = shm_at(shmid_imu);
	p_shm_imu = (SHM_IMU *)shm_tmp;
}

static void init_glbs()
{
	pid_main = getpid();
	sem_P(semid_glbs);
	p_shm_GLB->PID.PID_DATA = 0;
	p_shm_GLB->PID.PID_IMU = 0;
	p_shm_GLB->PID.PID_MAIN_TCP = 0;
	p_shm_GLB->PID.PID_MAIN = pid_main;

	p_shm_GLB->proc_status.en_dsp = 0;
	p_shm_GLB->proc_status.en_rcg = 0;
	p_shm_GLB->proc_status.en_tcp = 0;
	p_shm_GLB->proc_status.proc_TCP_status = STATUS_OFF;
	p_shm_GLB->proc_status.proc_status_proc = STATUS_ON;
	p_shm_GLB->proc_status.proc_ACT_status = STATUS_OFF;
	p_shm_GLB->proc_status.T_status = STATUS_OFF;
	p_shm_GLB->proc_status.ACT_OVER = 0;
	sem_V(semid_glbs);
}
static void proc_monitor()
{
	while (1)
	{
		usleep(1000);
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}
	}
}

static void  init_proc()
{
    pid_t pid;
	#ifdef online
    if((pid = fork()) < 0)
    {
        perror("fork error!");
    }
    else if(pid == 0)
    {
        main_data();
    }
    else
    {
       proc_monitor();
    }
	#else
	if((pid = fork()) < 0)
    {
        perror("fork error!");
    }
    else if(pid == 0)
    {
        main_IMU();
    }
    else
    {
        if((pid = fork()) < 0)
        {
            perror("fork error!");
        }
        else if(pid == 0)
        {
            main_data();
        }
        else
        {
            proc_monitor();
        }
    }
	#endif
}
int main()
{
    init_sig();		
	init_sem();		
	init_shm();		
	init_glbs();
	init_proc();
}