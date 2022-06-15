#include "shm.h"

#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>

//生成共享内存描述符
int shm_creat(int size)
{
    int shm = shmget(IPC_PRIVATE,size, 0666|IPC_CREAT);
    if(shm == -1)
    {
        printf("shmget error!\n");
        exit(1);
    }
    return shm;
}

//连接共享内存到进程
void* shm_at(int shm_id)
{
    void* shmptr = NULL;
    shmptr = shmat(shm_id,NULL,0);
    if(shmptr == (void*)-1)
    {
        perror("shmat error!\n");
        exit(1);
    }
    return shmptr;
}

//断开链接到共享内存
void shm_dt(void* shm_ptr)
{
    if((shmdt(shm_ptr)) == -1)
    {
        printf("remove shmdt error!\n");
		exit(1);
    }
}
//删除共享内存
void shm_remove(int semid,int shmid,void *shm_ptr)
{
    if ((semctl(semid, IPC_RMID, 0)) == -1)
	{
		perror("semctl error:");
		exit(1);
	}
	
    shm_dt(shm_ptr);
	if ((shmctl(shmid, IPC_RMID, 0)) == -1)
	{
		printf("remove shm error!\n");
		exit(1);
	}

}