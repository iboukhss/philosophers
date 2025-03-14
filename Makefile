MAKEFLAGS += --no-print-directory

NAME   ?= philo
OBJDIR ?= build/release

srcs := ph_main.c ph_simulation.c ph_queue.c ph_monitor.c ph_waiter.c ph_routine.c ph_utils.c
hdrs := ph_main.h
objs := $(addprefix $(OBJDIR)/, $(srcs:.c=.o))
deps := $(objs:.o=.d)

.PHONY: all _target release debug clean fclean re

all: debug asan tsan

_target: $(NAME)

$(NAME): $(objs)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

-include $(deps)

release:
	$(MAKE) NAME=philo OBJDIR=build/$@ CFLAGS="-Wall -Wextra -Werror" _target

debug:
	$(MAKE) NAME=philo-$@ OBJDIR=build/$@ CFLAGS="-Wall -Wextra -ggdb3" _target

asan:
	$(MAKE) NAME=philo-$@ OBJDIR=build/$@ CFLAGS="-Wall -Wextra -ggdb3 -fsanitize=address,undefined" _target

tsan:
	$(MAKE) NAME=philo-$@ OBJDIR=build/$@ CFLAGS="-Wall -Wextra -ggdb3 -fsanitize=thread,undefined" _target

norm:
	-norminette $(srcs) $(hdrs)

clean:
	rm -rf build

fclean: clean
	rm -f $(NAME) $(NAME)-*

re:
	$(MAKE) fclean
	$(MAKE) all
