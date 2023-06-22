SRCS = Request.cpp MemRequest.cpp main.cpp

NAME = webserv

CC = c++
CFLANG = -Wall -Wextra -Werror -g3 -fsanitize=address -std=c++98
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME) :
	$(CC) $(CFLANG) $(SRCS)  -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re