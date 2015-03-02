flags = -Wall -pedantic -std=gnu11 -lpthread
links = -lpthread
compiler = gcc
progs = queue.c
all:
	$(compiler) $(flags) $(progs) -o queue.out $(links) && ./queue.out


#for debuging. after start type <run>. and then <backtrace> 
debug:
	$(compiler) $(flags) $(progs) -o queue.out $(links) && gdb ./queue.out
