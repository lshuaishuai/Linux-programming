cc=g++
LD=-DMYSELF
.PHONY:all
all:calServer calClient

calServer:calServer.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp ${LD}
calClient:calClient.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp ${LD}

.PHONY:clean
clean:
	rm -f calClient calServer