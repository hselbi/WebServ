CXX = c++

SRCS = main.cpp\
		Request.cpp\
		Response.cpp\
		Socket.cpp\

OBJ =$(subst .cpp,.o,$(SRCS))

CXXFLAGS = -Wall -Wextra -Werror -std=c++98  #-g3 -fsanitize=address

NAME = server

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS)  $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

all : $(NAME)

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(NAME)

re: fclean all