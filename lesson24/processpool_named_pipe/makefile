.PHONY:all
all:receiver sender
receiver:receiver.cc
	g++ -o $@ $^ -std=c++11 -g
sender:sender.cc
	g++ -o $@ $^ -std=c++11 -g
.PHONY:clean
clean:
	rm -rf receiver sender