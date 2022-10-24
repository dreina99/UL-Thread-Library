lib 	:= libuthread.a
targets := $(lib)
objs	:= queue.o uthread.o preempt.o private.o 

CC 		:= gcc
CCFLAGS := -Wall -Wextra -Werror -MMD
CCFLAGS	+= -g

all: $(targets)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

%.o: %.c
	@echo "CC $@"
	$(CC) $(CCFLAGS) -c -o $@ $<

$(lib): $(objs)
	@echo "AR $@"
	@ar rcs $(lib) $^

clean:
	@echo "CLEAN"
	@rm -f $(targets) $(objs) $(deps)