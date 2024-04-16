.PHONY:all 
all:mysignal mytest

mytest:mytest.cc
	g++ -o $@ $^ -std=c++11 -g
mysignal:mysignal.cc
	g++ -o $@ $^ -std=c++11 -g
.PHONY:clean
clean:
	rm -rf mysignal mytest