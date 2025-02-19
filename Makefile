PROJ_NAME = philo
PROJ_SRCS = ph_main.c
PROJ_HDRS = ph_main.h
PROJ_OBJS = $(PROJ_SRCS:.c=.o)
PROJ_DEPS = $(PROJ_OBJS:.o=.d)

CFLAGS += -Wall -Wextra -g3 -MMD
CFLAGS += -fsanitize=thread
CPPFLAGS =
LDLIBS =
LDFLAGS = -pthread

.PHONY: all clean fclean re

all: $(PROJ_NAME)

clean:
	$(RM) $(PROJ_OBJS) $(PROJ_DEPS)

fclean: clean
	$(RM) $(PROJ_NAME)

re: fclean all

norm:
	-norminette $(PROJ_SRCS) $(PROJ_HDRS)

$(PROJ_NAME): $(PROJ_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(PROJ_NAME) $(PROJ_OBJS) $(LDLIBS) $(LDFLAGS)

-include $(PROJ_DEPS)
