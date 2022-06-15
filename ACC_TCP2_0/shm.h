#pragma once

int shm_creat(int size);

void* shm_at(int shm_id);
void shm_dt(void* shm_ptr);
void shm_remove(int semid,int shmid,void *shm_ptr);