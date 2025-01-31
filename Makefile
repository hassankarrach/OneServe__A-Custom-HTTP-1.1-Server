# SRCS ============>
CONFIG = ${addprefix ./src/config/, config_parser.cpp}
HTTP = ${addprefix ./src/http/, cgi_handler.cpp errors.cpp request.cpp response.cpp HttpDelete.cpp HttpGet.cpp HttpPost.cpp request_parser.cpp request_parser_utils.cpp}
SERVER = ${addprefix ./src/server/, server.cpp client.cpp connection_pool.cpp t_data.cpp ParsingConfigFile.cpp}
UTILS = ${addprefix ./src/utils/, utils.cpp logger.cpp}
TYPES = ${addprefix ./src/types/, httpTypes.cpp reqTypes.cpp resTypes.cpp}

SRC = ${CONFIG} ${HTTP} ${SERVER} ${UTILS} ${TYPES} ./src/main.cpp
# ==================

# OBJS ============>
NAME = webserv
OBJ = $(SRC:.cpp=.o)
CC = g++
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 
# ==================


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re