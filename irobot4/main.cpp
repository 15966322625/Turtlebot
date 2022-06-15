#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "api.h"
#include "server.h"

irobot robot;
Server server;

pthread_t tid_server = 0;

int pipe_fd[2];

//宏
#define STOP            0
#define JUST_GO         1
#define TURN_AROUND     2
#define GO_BACK         3
#define RESTART         4
#define GO_AND_STOP     5

void SigH(int sig)
{
    switch (sig)
    {
        case SIGINT:
            printf("recv SIGINT\r\n");
            server.tcp_close();
            exit(-1);
            break;
        default:
            break;
    }
}

void init_timer(void)
{
    struct itimerval timer_val;
    timer_val.it_value.tv_sec = 0;
	timer_val.it_value.tv_usec = 1000;			//每PER_SAMPLING ms发送一个信号
	timer_val.it_interval.tv_sec = 0;
	timer_val.it_interval.tv_usec = 1000;		//复位时间PER_SAMPLING ms
	setitimer(ITIMER_REAL, &timer_val, NULL);
}

int main(void)
{
    robot.init();
    robot.start();
    robot.mode_set(SafeMode);
    int distance;

    signal(SIGINT, SigH);

    if (pipe(pipe_fd) < 0)
    {
        printf("pipe create error\r\n");
        exit(-1);
    }
    char pipe_buf = 0;
    pthread_create(&tid_server, NULL, server.server_main, &server);
    char last_label=255;//接收最新的标签
    while(1)
    {
        read(pipe_fd[0], &pipe_buf, 1);
        // int pipe_scanf;
        // scanf("%d", &pipe_scanf);
        // pipe_buf = (char)pipe_scanf;
        switch(pipe_buf)//从管道中读取数据
        {
            case JUST_GO://直走
            {
                if(last_label == GO_BACK)//如果此时的指令是返回 先进行旋转再直走
                {
                    robot.turn_and_stop(179, 1, 1);
                }
                last_label = JUST_GO;
                printf("JUST GO\r\n");
                robot.drive_straight(100);
                break;
            }  
            case STOP:
            {
                last_label = STOP;
                printf("STOP\r\n");
                robot.stop();
                distance = robot.get_distance();
                break;
            } 
            case TURN_AROUND:
            {
                last_label=TURN_AROUND;
                printf("TURN_AROUND\r\n");
                robot.turn_and_stop(179, 1, 1);
                break;
            }
            case GO_BACK:
            {
                last_label=GO_BACK;
                printf("GO_BACK\r\n");
                printf("turn_and_stop = %d\r\n", robot.turn_and_stop(179, 1, 1));
                robot.drive_straight(100);
                char lable = 0;
                //server.tcp_send(&lable, 1);
                break;
            }
            case RESTART:
            {
                printf("RESTART\r\n");
                robot.start();
                robot.mode_set(SafeMode);
                break;
            }
            case GO_AND_STOP:
            {
                printf("GO_AND_STOP\r\n");
                robot.drive_straight_and_stop(200, 100, 1);
                char lable = 0;
                server.tcp_send(&lable, 1);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

