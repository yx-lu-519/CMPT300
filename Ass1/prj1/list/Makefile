CC=cc
CXX=CC
CCFLAGS= -g -std=c99 -Wall -Werror


all: test_list test

%.o : %.c
	$(CC) -c $(CCFLAGS) $<


test_list: list.o test_list.o
	$(CC) -o test_list list.o test_list.o

test: test_list
	./test_list


clean:
	rm -f core *.o test_list

