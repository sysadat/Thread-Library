targets := libuthread.a
objs := main.o

CC := gcc
CFLAGS := -Wall -Wextra -Werror
CFLAGS += -g

ifneq ($(V),1)
Q = @
endif

all: $(targets)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: $(objs)
	@echo "CC $@"
	$(Q)$(CC)$(CFLAGS) -o $@$^
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC)$(CFLAGS) -c -o $@$<$(DEPFLAGS)
clean:
	@echo "clean"
	$(Q)rm -f $(targets) $(objs) $(deps)
