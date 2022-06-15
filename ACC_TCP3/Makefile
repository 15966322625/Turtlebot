#������
CC     = g++
#�����ļ�����Ŀ¼
INCLUDE = .
#����ѡ��
#CFLAGS = -O -Wall
CFLAGS = -g -Wall
#Object�ļ�
OBJ    = 	main.o	\
			global.o\
			sem.o\
			shm.o\
			main_data.o\
			IMU_class.o\
			main_IMU.o\
			IMU_main.o\
			IMU_UDP.o\
			my_err.o
		



#-include <filename>
APP_tst:$(OBJ)
	@echo Linking......
	@$(CC) -Wall -o  APP_tst $(OBJ) -lwiringPi -lm -lpthread 
	@echo Complete !!!


%.o:%.cpp
	@echo Compiling $^ .......
	@$(CC) -Wall -c  $^ -lm -lpthread -lwiringPi 
	@echo OK!!!
	

	
.PHONY:cleanall cleanobj cleandiff clean
clean:
	rm -rf APP *.o
cleanall:cleanobj cleandiff
	@echo Cleaning all......
	@-rm APP_tst
	@echo Complete!!!
cleanobj:
	@echo Cleaning Obj......
	@-rm *.o
	@echo Complete!!!
cleandiff:
	@echo Cleaning Diff......
	@-rm *.diff
	@echo Complete!!!
