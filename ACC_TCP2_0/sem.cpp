#include "sem.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>



union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

//创建信号量集，返回信号量集合描述符
int sem_creat()
{
    int sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(sem_id == -1)
    {
        printf("semget sss error\n");
		exit(-1);
    }
    return sem_id;
}

//初始化信号量集合中的信号量的初始值 默认赋值为1
void sem_init(int sem_id)
{
    semun sem_union;
    sem_union.val =1;//初始值赋值为1
    if(semctl(sem_id,0,SETVAL,sem_union) == -1)
    {
        perror("Semset error!\n");
		exit(1);
    }
}

//对信号量集合p操作，默认对信号量集合中第一个
void sem_P(int sem_id)
{
    struct sembuf sem_b;
    sem_b.sem_num = 0; //信号量集合中第0个
	sem_b.sem_op = -1; //减一
	sem_b.sem_flg = SEM_UNDO;

    if(semop(sem_id,&sem_b,1) == -1)
    {
        printf("semid: %d, semop_P failed errno = %d\n", sem_id, errno);
		exit(1);
    }

}


//对信号量集合V操作，默认对信号量集合中第一个
void sem_V(int sem_id)
{
    struct sembuf sem_b;
    sem_b.sem_num = 0; //信号量集合中第0个
	sem_b.sem_op = 1; //减一
	sem_b.sem_flg = SEM_UNDO;

    if(semop(sem_id,&sem_b,1) == -1)
    {
        printf("semid: %d, semop_v failed errno = %d\n", sem_id, errno);
		exit(1);
    }
}

//删除信号量集合
void sem_remove(int sem_id)
{
    if ((semctl(sem_id, IPC_RMID, 0)) == -1)
	{
		perror("semctl error:");
		exit(1);
	}
}