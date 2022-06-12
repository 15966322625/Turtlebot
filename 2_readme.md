# Turtlebot
基于Cpp11实现的姿态解算指令控制机器人
## 项目框架
姿态机器人受人体动作的控制，IMU传感器通过解算人体动作的角度和角速度，进行动作判别：停止 前进 原地旋转180度 后退。
整套系统的实现分为人为信号的采集端和指令的执行端。

## 实现技术
- 自定义实现IMU数据采集类、IMU控制类、UDP通信类。
- 使用多线程实现对数据的循环采集，使用信号量和共享变量实现通信和数据同步。
- 自定义进行错误情况、共享数据等的封装。
- 在UDP下实现IMU传感器的无线采集。
- 利用TCP\IP实现两个终端之间的交互。
- 在指令执行端多次使用结构体、全局变量实现动作指令的封装，极大的简化了指令的施行。

## 环境要求
- Linux
- C++11
- UDP局域网下
- Turtlebot机器人
## Turtlebot信号采集端（ACC_TCP3_0)--目录树
```
.
├── bin
│   └── Turtlebot 可执行文件
├── build
│   └── Makefile
├── code             源代码
│   ├── buffer       自动扩容的缓冲区
│   ├── config       配置文件
│   ├── global       全局变量、自定义结构体
│   ├── IMU_class       自定义实现IMU数据采集类
│   ├── IMU_main       自定义实现IMU的采集动作
│   ├── IMU_UDP     自定义实现UDP下传感器的控制类
│   ├── main_data       实现对采集数据的处理
│   ├── main_IMU   实现对IMU传感器的控制
│   ├── main   主程序 程序入口
│   ├── my_err     实现一些常见错误的封装
│   ├── sem        实现信号量函数的封装
│   └── shm        封装一些共享变量
├── log              日志目录
├── Makefile
├── README.md
└── resources        静态资源
```

## Turtlebot指令执行端(irobot)--目录树
```
.
├── bin
│   └── Turtlebot 可执行文件
├── build
│   └── Makefile
├── code             源代码
│   ├── main       程序入口
│   ├── server     机器人指令执行的一端也可以看作是服务器端，实现对信号采集端的指令响应。
│   ├── api       负责多个动作指令的具体实现
│   └── shm        封装一些共享变量
├── log              日志目录
├── Makefile  
├── README.md
└── resources        静态资源
```

