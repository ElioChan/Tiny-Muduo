OBJ1 = server.cpp Epoll.cpp Socket.cpp InetAddress.cpp util.cpp Channel.cpp Server.cpp Eventloop.cpp Connection.cpp Acceptor.cpp
OBJ2 = client.cpp util.cpp
all: server client

server:${OBJ1}
	@echo "开始编译"
	g++ -o $@ $?
	@echo "编译结束"

client:${OBJ2}
	@echo "开始编译"
	g++ -o $@ $?
	@echo "编译结束"

clean:
	rm server && rm client