#include "main_IMU.h"
#include "global.h"
#include "shm.h"
#include "sem.h"
#include "IMU_class.h"

IMU_class IMU;

static pid_t pid_imu;
static int proc_status = 1;


static void APP_exit()
{
	printf("=============== IMU  EXIT! ==============\n ");
	exit(0);
}


static void SigH(int Sig)
{
	switch (Sig)
	{
	case SIGINT:
		break;
	case SIGUSR1:
		break;
	case 12:
		proc_status = STATUS_OFF;
		break;
	default:
		break;
	}
}

static void init_sig()
{
	signal(SIGINT, SigH);
	signal(10, SigH);
	signal(12, SigH);
}

static void init_glbs()
{
	pid_imu = getpid();
	sem_P(semid_glbs);
	p_shm_GLB->PID.PID_IMU = pid_imu;
	sem_V(semid_glbs);
}

void main_IMU()
{
    float imu_data[IMU_CH][12] = {0};
    init_sig();
	init_glbs();

    IMU.IMU_init();


    while (1)
	{
		usleep(6000);
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}
		IMU.IMU_getdata(imu_data);
        sem_P(semid_imu);
        for(int ch=0;ch<IMU_CH;ch++)
        {
            for(int i =0;i<12;++i)
            {
                p_shm_imu->data[ch][i] = imu_data[ch][i];
            }
        }
        p_shm_imu->flag = true;
        sem_V(semid_imu);
	}
}