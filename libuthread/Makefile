# Target library
lib  := libuthread.a
objs := queue.o

CC     := gcc
STATIC := ar
STATIC += rcs

CFLAGS := -Wall -Wextra -Werror
CFLAGS += -g

ifneq ($(V),1)
Q = @
endif

all: $(lib)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

$(lib): $(objs)
	@echo "CC $@"
	$(Q)$(STATIC) $@ $^
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)
clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)
