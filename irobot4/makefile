TARGET := robot
CC := g++

CROSS_COMPILE ?= arm-linux-gnueabihf-
#交叉编译器
TARGET        ?= main
#最终目标文件
##CC            :=$(CROSS_COMPILE)g++
CC            :=g++
#gcc/g++

INCUDIRS      :=.\
				api\
				server
#所有包含.h的目录

SECDIRS       :=.\
				api\
				server
#所有包含.cpp的目录

INCLUDE		  :=$(patsubst %, -I %, $(INCUDIRS))
#把所有的目录前面加上-I 如 ADC 变成 -I ADC

CFILES        :=$(foreach dir, $(SECDIRS), $(wildcard $(dir)/*.cpp))
#对于SECDIRS中的每个成员目录，搜索目录下的所有.cpp文件
#foreach用于遍历
#wildcard 用于让通配符在函数变量中展开
#如 ADC 中 有 ADC.cpp

CFILENDIR     :=$(notdir $(CFILES))
#将CFILES的所有成员 去掉目录
#如 ADC/ADC.cpp 变成 ADC.cpp

COBJS         :=$(patsubst %, obj/%, $(CFILENDIR:.cpp=.o))
#将CFILENDIR的所有成员的后缀名 .cpp 换成.o 再在前面加上obj/
#如 ADC.cpp 变成 obj/ADC.o

VPATH         :=$(SECDIRS)
#源码文件在VPATH路径下寻找

#all : $(TARGET)
.PHONY : clean

$(TARGET) : $(COBJS)
	$(CC) -o $@ $^ -lwiringPi -lpthread -Wall -lcrypt -lrt -lm -O1
# $@表示所有的目标文件
# $^表示所有依赖文件的集合

$(COBJS) : obj/%.o : %.cpp
	$(CC) -c $(INCLUDE) -o $@ $< -lwiringPi -lpthread -Wall -lcrypt -lrt -lm
# $< 表示所有依赖文件 一个一个取出

clean:
	rm -rf $(TARGET) $(COBJS)
