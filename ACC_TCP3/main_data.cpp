#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fstream>
#include "configure.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/shm.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "main_data.h"
#include "shm.h"
#include "sem.h"
#include "global.h"
#include "IMU_main.h"


using namespace std;

static pid_t pid_dsp = 0;
static pid_t pid_DATA = 0;
static pid_t pid_main = 0;
static pid_t pid_tcp = 0;
static pid_t pid_rcg = 0;

static int ms_T_count = 0;

static bool process_status = true;
static int TCP_status = 0;
static std::ofstream file_IMU;
static SHM_IMU_data *local_data = new SHM_IMU_data;
static std::ofstream ans;


//使用无线IMU
#ifdef online

imu_udp imu_0(5000,0);

imu_udp* imu_list[IMU_CH] = {&imu_0};
imu_thread imu_proc;

#endif
//进程结束处理函数
static void APP_exit()
{

    file_IMU.close();
    ans.close();
    cout<<"=============== DATA EXIT! =============="<<endl;
    exit(0);
} 
static void init_udp_imu()
{
    
    #ifdef online
    imu_proc.imu_thread_init(imu_list, IMU_CH);
	while(!imu_0.connect_flag)
{
usleep(1000);
}
     #endif 
}
//初始化进程ID
static void init_glbs()
{
    pid_DATA = getpid();//获得当前进程id;
    //进程id更新到共享内存中
    sem_P(semid_glbs);
    p_shm_GLB->PID.PID_DATA = pid_DATA;
    sem_V(semid_glbs);
    //获取main和dsp的进程id
    while(pid_main == 0)
    {
        sem_P(semid_glbs);

		pid_main = p_shm_GLB->PID.PID_MAIN;

        pid_tcp = p_shm_GLB->PID.PID_MAIN_TCP;
        sem_V(semid_glbs);
    }
}

// static void wait_TCP()
// {
// 	do 
// 	{
// 		usleep(300);
// 		Sem_P(semid_glbs);
// 		TCP_status = p_shm_GLB->proc_status.proc_TCP_status;
// 		Sem_V(semid_glbs);
// 	} while (TCP_status == 0);
// 	printf("TCP.........OK!!!\n");
// }
static void wait_IMU()
{
	bool imu__data_flag = false;
    do 
	{
		usleep(300);
		sem_P(semid_imu);
		imu__data_flag = p_shm_imu->flag;
		sem_V(semid_imu);
	} while (!imu__data_flag);
	printf("IMU.........OK!!!\n");
}

//初始化定时器
static void init_timer()
{
    struct itimerval timer_val;
	timer_val.it_value.tv_sec = 0;
	timer_val.it_value.tv_usec = 1000;				
	timer_val.it_interval.tv_sec = 0;
	timer_val.it_interval.tv_usec = 1000;			//1ms触发一个信号
	setitimer(ITIMER_REAL, &timer_val, NULL);
}
//信号响应函数
static void SigH(int Sig)
{
    switch(Sig)
    {
        case SIGINT:
            break;
        case SIGALRM:
        {
            ms_T_count += 1;
            break;
        }
        case SIGUSR1:
            break;
        case SIGUSR2:
            {
                process_status = false;
                break;
            }
    }
}
//绑定信号和函数
static void init_sig()
{
    signal(SIGINT, SigH);
	signal(SIGUSR1, SigH);
	signal(SIGALRM, SigH);
	signal(SIGUSR2, SigH);
}
static void init_file()
{
    
    file_IMU.open(path_file);
    if(!file_IMU.is_open())
    {
        cout<<"open save imu file error\n";
        return;
    }
    file_IMU<<',';
    for(int i=0;i<IMU_CH-1;++i)
    {
        file_IMU<<"ACC_x,ACC_y,ACC_z,GYR_x,GYR_y,GYR_z,Q_0,Q_1,Q_2,Q_3,Pitch,Yaw,";
    }
    file_IMU<<"ACC_x,ACC_y,ACC_z,GYR_x,GYR_y,GYR_z,Q_0,Q_1,Q_2,Q_3,Pitch,Yaw\n";
    file_IMU.flush();

    ans.open("./ans.csv");
}
//记录采集的IMU原始数据
static void OFFline_IMU(int time,float data[IMU_CH][12])
{
    //#ifdef online
    file_IMU<<time<<',';
    for(int j=0;j<IMU_CH*12-1;j++)
    {
        file_IMU<<data[j/12][j%12]<<',';
    }
    file_IMU<<data[IMU_CH-1][11]<<endl;
    //#endif
}


//拷贝IMU数据
void CopyFromData(SHM_IMU_data *imu_data, float data[IMU_CH][12], int tnow)
{
    int number = imu_data->count;
    for(int ch=0;ch<IMU_CH;++ch)
    {
        imu_data->Data_IMU[ch].ACC_x[number] = data[ch][0];
        imu_data->Data_IMU[ch].ACC_y[number] = data[ch][1];
        imu_data->Data_IMU[ch].ACC_z[number] = data[ch][2];

        // imu_data->Data_IMU[ch].GYRO_x[number] = data[ch][3];
        // imu_data->Data_IMU[ch].GYRO_y[number] = data[ch][4];
        // imu_data->Data_IMU[ch].GYRO_z[number] = data[ch][5];

        // imu_data->Data_IMU[ch].Q_0[number] = data[ch][6];
        // imu_data->Data_IMU[ch].Q_1[number] = data[ch][7];
        // imu_data->Data_IMU[ch].Q_2[number] = data[ch][8];
        // imu_data->Data_IMU[ch].Q_3[number] = data[ch][9];

        // imu_data->Data_IMU[ch].Pitch[number] = data[ch][10];
        // imu_data->Data_IMU[ch].Yaw[number] = data[ch][11];
    }
    imu_data->count++;
    imu_data->T_now = tnow;
}

//100HZ采集IMU数据
void CollectImuData(int time,bool& new_data)
{
    static int last_time = 0;
    float imu_data[IMU_CH][12] = {0};
    float filter_data[IMU_CH][12] = {0};
    if(time-last_time >= frequence)
    {
        last_time = time;
        //采集IMU数据

        #ifdef online
        imu_proc.get_data(imu_data,new_data);
        #else
        sem_P(semid_imu);
        for(int ch=0;ch<IMU_CH;ch++)
        {
            for(int i =0;i<12;++i)
            {
                imu_data[ch][i] = p_shm_imu->data[ch][i];
            }
        }
        sem_V(semid_imu);
        #endif
        //数据拷贝到结构体中
        CopyFromData(local_data,imu_data,time);
        //保存数据
        OFFline_IMU(time,imu_data);
    }   
}

static int  desicion(SHM_IMU_data *imu_data)
{
    static int last_count =0;
    //求一个窗的平均值
    float sum =0.0;
    for(int i=0;i<imu_data->count;++i)
    {
        sum +=  imu_data->Data_IMU[0].ACC_x[i];
    }
    sum = sum/(float)imu_data->count;
    ans<<sum<<',';
    // if(sum>= LOW && sum<=HIGH)
    // {
    //     last_count++;
    // }
     if(sum<=0.5)
    {
        last_count++;
    }
    else
    {
        last_count = 0;
    }
    if(last_count>=4)
    {
        ans<<1<<std::endl;
        return 1;
    }
    else
    {
        ans<<0<<std::endl;
        return 0;
    }
}


//updown---倒车---机器人返回
static int updown(SHM_IMU_data *imu_data)
{
    static int last_count =0;
    //求一个窗的平均值
    float sum =0.0;
    for(int i=0;i<imu_data->count;++i)
    {
        sum +=  imu_data->Data_IMU[0].ACC_z[i];
    }
    sum = sum/(float)imu_data->count;
    ans<<sum<<',';
    int ans=0;
    if(sum<=-0.5) ans =3;
    else ans=2;
    return ans;
    
    //  if(sum<=0.5)
    // {
    //     last_count++;
    // }
    // else
    // {
    //     last_count = 0;
    // }
    // if(last_count>=4)
    // {
    //     ans<<1<<std::endl;
    //     return 1;
    // }
    // else
    // {
    //     ans<<0<<std::endl;
    //     return 0;
    // }

}


static int init_tcp()
{
    int sock_cli= socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(tcp_port);  
	servaddr.sin_addr.s_addr = inet_addr(ipaddr);  

    if(connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        return -1;
    }
    else
    return sock_cli;

}

void main_data()
{
    int tnow =0;
    int new_data_count =0;
    int real_ans=0;
    init_file();
    init_sig(); //初始信号
    init_glbs(); //初始全局变量
    #ifdef TCP
    int sock_cli = init_tcp();
    if(sock_cli < 0)
    {
        perror("connect fail !!!");
		exit(1);
    }
    else
    {
        printf("网络连接成功>>>>>\n");
    }
    #endif
    #ifdef online
    init_udp_imu();
    #else
    wait_IMU(); //等待IMU
    #endif
    
    init_timer(); //初始并开启定时器
    printf("main DATA ok\n");
    while (true)
    {
        if(process_status == false)
        {
            APP_exit();
        }
        tnow = ms_T_count;
        bool new_data = false;
        CollectImuData(tnow,new_data);//采集imu数据
        if(local_data->count >= 4)
        {
            static int last_ans = 10;

            int ans = desicion(local_data);
            if(!new_data)
            {
                ++new_data_count;
                if(new_data_count>=15)
                {
                    //new_data_count = 0;
                    ans =0;
                    printf("不是新数据\n");
                }
                
            }
            else
            {
                new_data_count = 0;
            }

            if(ans == 1)
            {
                int ud = updown(local_data);
                real_ans = ud;//ud 是2或3
                //     char buf[2];
                //     buf[0] = '0';
                //     buf[1] = '\0';
                //     #ifdef TCP
                //     if(write(sock_cli,buf,2)<0)
                //     {
                //         perror("断开连接\n");
                //         exit(1);
                //     }
                //     #endif
                //    //
            }
            else
            {
                real_ans = 0;
            }
            if(real_ans != last_ans)
            {
                last_ans = real_ans;
                switch (real_ans)
                {
                case 0:
                {
                    char buf[2];
                    buf[0] = '0';
                    buf[1] = '\0';
                    #ifdef TCP
                    if(write(sock_cli,buf,2)<0)
                    {
                        perror("断开连接\n");
                        exit(1);
                    }
                    #endif
                    break;
                }
                case 2:
                {
                    char buf2[2];
                    buf2[0] = '2';
                    buf2[1] = '\0';
                    #ifdef TCP
                    if(write(sock_cli,buf2,2)<0)
                    {
                        perror("断开连接");
                        exit(1);
                    }
                    #endif
                    break;
                }
                case 3:
                {
                    char buf3[2];
                    buf3[0] = '3';
                    buf3[1] = '\0';
                    #ifdef TCP
                    if(write(sock_cli,buf3,2)<0)
                    {
                        perror("断开连接");
                        exit(1);
                    }
                    #endif
                    break;
                }
                default:
                    break;
                }
                printf("结果是： %d!\n",real_ans);
            }
            local_data->count = 0;
            local_data->T_now = 0;
        }
        usleep(100);
    }
}
