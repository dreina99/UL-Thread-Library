lib 	:= libuthread.a
targets := $(lib)
objs	:= queue.o uthread.o preempt.o context.o sem.o

CC 		:= gcc
CCFLAGS := -Wall -Wextra -Werror -MMD
CCFLAGS	+= -g

# Current directory
CUR_PWD := $(shell pwd)

all: $(targets)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

%.o: %.c
	@echo "CC	$@"
	@$(CC) $(CCFLAGS) -c -o $@ $<

$(lib): $(objs)
	@echo "AR	$@"
	@ar rcs $(lib) $^

clean:
	@echo "CLEAN	$(CUR_PWD)"
	@rm -f $(targets) $(objs) $(deps)