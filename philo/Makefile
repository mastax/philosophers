NAME = philo 
SRCS = ft_free.c \
		ft_init.c \
		ft_philo.c \
		main.c \
		utils.c \
		monitor.c \
		destroy_resources.c

HEADER = philo.h

CC = cc
FLAGS = -Wall -Wextra -Werror
RM = rm -f

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

re: fclean all

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)