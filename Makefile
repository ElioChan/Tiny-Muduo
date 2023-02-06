OBJ1 = server.cpp src/Epoll.cpp src/Socket.cpp src/InetAddress.cpp src/util.cpp src/Channel.cpp \
       src/Server.cpp src/Eventloop.cpp src/Connection.cpp src/Acceptor.cpp src/Buffer.cpp
OBJ2 = client.cpp src/util.cpp
all: server client
# all用于make多个目标时 此文档执行makefile后等价于 makefile all = makefile server & makefile client

server:${OBJ1}
	@echo "开始编译"
	g++ -o $@ $?
	@echo "编译结束"

client:${OBJ2}
	@echo "开始编译"
	g++ -o $@ $?
	@echo "编译结束"

# $@  表示目标文件
# $^  表示所有的依赖文件
# $<  表示第一个依赖文件
# $?  表示比目标还要新的依赖文件列表

clean:
	rm server && rm client