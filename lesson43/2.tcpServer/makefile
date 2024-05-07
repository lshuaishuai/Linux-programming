cc=g++
.PHONY:all
all:tcpServer tcpClient

tcpServer:tcpServer.cc
	$(cc) -o $@ $^ -std=c++11

tcpClient:tcpClient.cc
	$(cc) -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f tcpClient tcpServer