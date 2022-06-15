#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <stdint.h>
#include "api.h"

int irobot::init(void)
{
    wiringPiSetup();
#if UART
    this->serial_fd = serialOpen(SERIAL_PORT, 57600);
    if (this->serial_fd < 0)
    {
        printf("open serial err\r\n");
        return -1;
    }
    else
    {
        printf("open serial success at %s\r\n", SERIAL_PORT);
    }
#else
    printf("use consore\r\n");
#endif
    return 0;
}

int irobot::command_send(uint8_t* command, int command_len)
{
#if UART
    serialFlush(this->serial_fd);
    return write(this->serial_fd, command, command_len);
#else
    for (int i = 0; i < command_len; i++)
    {
        printf("%02x ", command[i]);
    }
    printf("\r\n");
        for (int i = 0; i < command_len; i++)
    {
        printf("%d ", command[i]);
    }
    printf("\r\n");
    return 0;
#endif
}

int irobot::data_receive(uint8_t* buf, int buf_len)
{
    return read(this->serial_fd, buf, buf_len);
}

int irobot::start()
{
    uint8_t opcode_start[1] = {128};
    return this->command_send(opcode_start, 1);
}

int irobot::stop()
{
    return this->drive(0, 0);
}

int irobot::mode_set(operating_mode mode)
{
    int ret = 0;
    uint8_t opcode_safe_mode[1] = {131};
    uint8_t opcode_full_mode[1] = {132};
    switch(mode)
    {
        case PassiveMode:
            break;
        case SafeMode:
            ret = this->command_send(opcode_safe_mode, 1);
            break;
        case FullMode:
            ret = this->command_send(opcode_full_mode, 1);
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}

int irobot::drive(int velocity, int radius)
{
    uint8_t command[5];
    uint8_t* p_v = NULL;
    uint8_t* p_r = NULL;

    command[0] = 137;
    p_v = (uint8_t*)&velocity;
    p_r = (uint8_t*)&radius;
    command[1] = p_v[1];
    command[2] = p_v[0];
    command[3] = p_r[1];
    command[4] = p_r[0];
    return this->command_send(command, 5);
}

int irobot::drive_straight(int velocity)
{
    return this->drive(velocity, 32768);
}

int irobot::turn_in_place(float velocity_r)
{
    int velocity = velocity_r * 130;
    if (velocity > 500 || velocity < -500)
    {
        printf("command out of range\r\n");
        return -1;
    }
    if (velocity >= 0)
    {
        return this->drive(velocity, 0x0001);
    }
    if (velocity < 0)
    {
        return this->drive(-velocity, 0xFFFF);
    }
    return 0;
}

int irobot::drive_straight_and_stop(int distance, int velocity, bool set_callback)
{
    uint8_t command[20];
    int distance_ = distance;
    uint8_t* p_v = NULL;
    uint8_t* p_d = NULL;
    int ret;
    p_v = (uint8_t*)&velocity;
    p_d = (uint8_t*)&distance_;


    command[0] = 152;           //表示脚本
    command[2] = 137;           //行驶
    command[3] = p_v[1];        //速度
    command[4] = p_v[0];
    command[5] = 0x80;          //半径特值
    command[6] = 0x00;
    command[7] = 156;           //等待距离
    command[8] = p_d[1];        //距离
    command[9] = p_d[0];
    command[10] = 137;          //行驶
    command[11] = 0;            //全部为零
    command[12] = 0;
    command[13] = 0;
    command[14] = 0;
    if (set_callback == 0)
    {
        command[1] = 13;        //长度
    }
    else
    {
        command[1] = 15;
        command[15] = 142;      //返回传感器值
        command[16] = 19;       //返回距离
    }
    ret = this->command_send(command, command[1] + 2);
    usleep(2000);
    command[0] = 153;
    ret = this->command_send(command, 1);
    if (set_callback == 0)
    {
        return ret;
    }
    else
    {
        serialFlush(this->serial_fd);
        uint8_t buf[2];
        this->data_receive(buf, 2);
        printf("received\r\n");
        return (int)(buf[0]<<8 | buf[1]);
    }
}

int irobot::turn_and_stop(int angle, float velocity_r, bool set_callback)
{
    int velocity = velocity_r * 130;
    uint8_t command[20];
    uint8_t radius_in[2];

    if (velocity >= 0)          //表示逆时针
    {
        radius_in[0] = 0x00;    //逆时针特值
        radius_in[1] = 0x01;
    }
    if (velocity < 0)           //表示顺时针
    {
        radius_in[0] = 0xFF;    //顺时针特值
        radius_in[1] = 0xFF;
        velocity = -velocity;   //取绝对值
    }

    uint8_t* p_a = NULL;
    uint8_t* p_v = NULL;
    int ret;
    p_a = (uint8_t*)&angle;
    p_v = (uint8_t*)&velocity;

    command[0] = 152;           //表示脚本
    command[2] = 137;           //行驶
    command[3] = p_v[1];        //速度
    command[4] = p_v[0];
    command[5] = radius_in[0];  //半径特值
    command[6] = radius_in[1];
    command[7] = 157;           //等待角度
    command[8] = p_a[1];        //角度
    command[9] = p_a[0];
    command[10] = 137;          //行驶
    command[11] = 0;            //停止
    command[12] = 0;
    command[13] = 0;
    command[14] = 0;
    if (set_callback == 0)
    {
        command[1] = 13;        //长度
    }
    else
    {
        command[1] = 15;
        command[15] = 142;      //返回传感器
        command[16] = 20;       //角度
    }

    ret = this->command_send(command, command[1] + 2);
    usleep(2000);
    command[0] = 153;
    ret = this->command_send(command, 1);
    if (set_callback == 0)
    {
        return ret;
    }
    else
    {
        uint8_t buf[2];
        this->data_receive(buf, 2);
        return (int)(buf[0]<<8 | buf[1]);
    }
}

int irobot::get_distance()
{
    uint8_t command[2];
    uint8_t buf[2];
    command[0] = 142;
    command[1] = 19;

    this->command_send(command, 2);
    this->data_receive(buf, 2);

    return (int)(buf[0]<<8 | buf[1]);

}
int irobot::get_angle()
{
    uint8_t command[2];
    uint8_t buf[2];
    command[0] = 142;
    command[1] = 20;

    this->command_send(command, 2);
    this->data_receive(buf, 2);

    return (int)(buf[0]<<8 | buf[1]);
}