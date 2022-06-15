#pragma once

//创建信号量集，返回信号量集合描述符
int sem_creat();

//初始化信号量集合中的信号量的初始值 默认赋值为1
void sem_init(int sem_id);

//对信号量集合p操作，默认对信号量集合中第一个
void sem_P(int sem_id);


//对信号量集合V操作，默认对信号量集合中第一个
void sem_V(int sem_id);

//删除信号量集合
void sem_remove(int sem_id);

