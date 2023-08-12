NAME = webserv

SRCS_REQ		= ./request
SRCS_CONF		= ./config
SRCS_POST		= ./postMethod
SRC_CGI			= ./core/cgi
SRC_CORE		= ./core/srcs
SRC_RESPONSE	= ./response


SRCS			= ./main.cpp \
					$(addprefix $(SRCS_REQ)/, Request.cpp MemRequest.cpp chunked.cpp) \
					$(addprefix $(SRCS_CONF)/, ConfLoca.cpp ConfServer.cpp Config.cpp Tools.cpp) \
					$(addprefix $(SRC_CORE)/, Client.cpp  Server.cpp  ) \
					$(addprefix $(SRC_CGI)/, Cgi.cpp) \
					$(addprefix $(SRC_RESPONSE)/, Response.cpp ResponseError.cpp ResponseGetters.cpp Utils.cpp ResponseMethods.cpp ResponseReader.cpp ResponseUtils.cpp ResponseSetters.cpp ResponseChecker.cpp)

OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
CWWFLAGS		= -Wall -Werror -Wextra -std=c++98
CFAFLAGS		= -std=c++98

all:			$(NAME)


$(NAME):		$(OBJS)
				$(CXX) $(CWWFLAGS) $(CFAFLAGS) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re
