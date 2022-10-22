lib 	:= libuthread.a
targets := queue.o $(lib)
objs	:= queue.o

CC 		:= gcc
CCFLAGS := -Wall -Wextra -Werror -MMD
CCFLAGS	+= -g

all: $(targets)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

%.o: %.c
	@echo "CC $@"
	@$(CC) $(CCFLAGS) -c -o $@ $<

$(lib): $(objs)
	@echo "AR $@"
	@ar rcs $(lib) $^

clean:
	@echo "CLEAN"
	@rm -f $(targets) $(objs) $(deps)