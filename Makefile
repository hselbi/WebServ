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
					$(addprefix $(SRC_RESPONSE)/, Response.cpp ResponseError.cpp ResponseGetters.cpp Utils.cpp ResponseMethods.cpp)

OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
CWWFLAGS		= -Wall -Werror -Wextra -std=c++98
CFAFLAGS		= -std=c++98 -g -fsanitize=address

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CFAFLAGS) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re