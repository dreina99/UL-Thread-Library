# Target library
lib := libuthread.a
CCFLAGS = -Wall -Wextra -Werror

all: queue.o $(lib) 

queue.o: queue.c
	gcc -g $(CCFLAGS) -c queue.c 

$(lib):
	ar rcs $(lib) queue.o

clean:
	rm -f libuthread.a queue.o