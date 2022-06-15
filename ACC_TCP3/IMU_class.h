#ifndef IMU_class_
#define IMU_class_
#include "configure.h"

class IMU_class
{
    public:
    IMU_class();
    ~IMU_class();
    void IMU_getdata(float data[IMU_CH][12]);
    void IMU_init();
    private:
    int IMU_fd;
    bool IMU_flag;
};

#endif