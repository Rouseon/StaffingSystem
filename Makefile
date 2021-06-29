# @Descripttion: 
# @version: 
# @Author: Lou Sheng
# @Date: 2021-06-25 14:50:26
# @LastEditors: Lou Sheng
# @LastEditTime: 2021-06-26 12:10:39

CC=gcc
CPTHREAD=-pthread
CSQL3=-lsqlite3
CFLAGS:= -I inc #编译依赖头文件
VPATH=inc:src	#虚路径，定位所有文件

all:server client	#all伪执行文件

server:server.o serverapi.o
	$(CC) $(CPTHREAD) $^ -o $@ $(CSQL3)
server.o:server.c
	gcc $(CPTHREAD) -c $^ -o $@ $(CSQL3) $(CFLAGS)
serverapi.o:serverapi.c
	gcc $(CPTHREAD) -c $^ -o $@ $(CSQL3) $(CFLAGS)

client:client.o clientapi.o
	$(CC) $(CPTHREAD) $^ -o $@ $(CSQL3)
client.o:client.c
	$(CC) $(CPTHREAD) -c $^ -o $@ $(CSQL3) $(CFLAGS)
clientapi.o:clientapi.c
	$(CC) $(CPTHREAD) -c $^ -o $@ $(CSQL3) $(CFLAGS)

.PHONY:cleanall
cleanall:
	rm *.o server client

.PHONY:clean 
clean:
	rm *.o

.PHONY:cleansql
cleansql:
	rm ./sql/*.db
