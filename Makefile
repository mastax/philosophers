NAME = philo 
SRCS = ft_free.c \
		ft_init.c \
		ft_philo.c \
		main.c \
		utils.c \
		monitor.c \
		special_case.c \
		destroy_resources.c \

HEADER = philo.h

CC = cc
FLAGS = -Wall -Wextra -Werror #-g -fsanitize=thread
RM = rm -rf

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) $(HEADER)
	$(CC) $(FLAGS) $(OBJS) -o$(NAME)

re: fclean all

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)