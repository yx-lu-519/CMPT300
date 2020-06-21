TARGET1 = array_stats_test
TARGET2 = process_ancestors_test
OBJS1 = array_stats_test.o
OBJS2 = process_ancestors_test.o

CFLAGS = -std=c99 -g -Wall -Werror -D _GNU_SOURCE -static
CC_C = gcc

all: array_stats_test process_ancestors_test

array_stats_test: array_stats_test.c
	gcc -std=c99 -g -Wall -Werror -D _GNU_SOURCE -static -o array_stats_test array_stats_test.c

process_ancestors_test: process_ancestors_test.c
	gcc -std=c99 -g -Wall -Werror -D _GNU_SOURCE -static -o process_ancestors_test process_ancestors_test.c

clean:
	rm -f $(TARGET1)
	rm -f $(TARGET2)
	rm -f $(OBJS1)
	rm -f $(OBJS2)

transfer:
	scp -p2222 $(TARGET1) root@localhost:$(TARGET1)
	scp -p2222 $(TARGET2) root@localhost:$(TARGET2)

killqemu:
	killall qemu-system-x86_64
