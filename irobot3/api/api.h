#ifndef __API_H
#define __API_H

#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <stdint.h>

#define SERIAL_PORT         "/dev/ttyUSB0"

#define UART 1

enum operating_mode
{
    PassiveMode,
    SafeMode,
    FullMode,
};

class irobot
{
private:
    int command_send(uint8_t* command, int command_len);
    int data_receive(uint8_t* buf, int buf_len);
    int serial_fd;
    bool waiting;
    bool waiting_for_callback;
public:
    int init(void);
    int start(void);
    int stop(void);
    int mode_set(operating_mode mode);
    int drive(int velocity, int radius);
    int drive_straight(int velocity);
    int turn_in_place(float velocity_r);
    int drive_straight_and_stop(int distance, int velocity, bool set_callback);
    int turn_and_stop(int angle, float velocity_r, bool set_callback);
    int get_distance();
    int get_angle();
};

#endif 