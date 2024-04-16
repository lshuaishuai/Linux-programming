.PHONY:all
all: mybin myexec 

mybin:mybin.c
	gcc -o mybin mybin.c -std=c99 
myexec:myexec.c
	gcc -o myexec myexec.c -std=c99 
.PHONY:clean
clean:
	rm -rf myexec mybin
