NAME = webserv

SRCS_REQ		= ./request
SRCS_CONF		= ./config
SRCS_POST		= ./postMethod
SRC_RESPONSE	= ./response
SRC_CORE	= ./core/srcs


SRCS			= ./main.cpp \
					$(addprefix $(SRCS_REQ)/, Request.cpp MemRequest.cpp) \
					$(addprefix $(SRCS_CONF)/, ConfLoca.cpp ConfServer.cpp Config.cpp Tools.cpp) \
					$(addprefix $(SRC_CORE)/, Client.cpp  Server.cpp  utils.cpp ) \
					$(addprefix $(SRC_RESPONSE)/, Response.cpp ResponseError.cpp Utils.cpp)

OBJS = $(SRCS:%.c=objs/%.o)

objs/%.o : %.c
        $(CC) -o $@ $(CFLAGS) $<

CXX				= clang++
# CXXFLAGS		= -Wall -Werror -Wextra -std=c++98
CXXFLAGS		= -std=c++98 -fsanitize=address
# CXXFLAGS		= -std=c++98 

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re