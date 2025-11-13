NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

SRC_DIR = srcs
OBJ_DIR = obj

CPP = main.cpp poll.cpp utils.cpp

OBJS = $(CPP:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

