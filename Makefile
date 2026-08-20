NAME = webserv

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++17
CXXFLAGS =  -MMD -MP -Iinclude
DEBUG_FLAGS = -g3 -fsanitize=address

RM = rm -f
RMDIR = rm -rf

SRC_DIR = src
OBJ_DIR = build

# main, loop
CORE = \
	main.cpp \

# all cgi files
CGI = \

# config parsing
CONFIG = MockConfig.cpp

# http response
HTTPRESPONSE = HttpResponse.cpp

# http request
HTTPREQUEST = \

# bytes, socket etc
NETWORK = \

# utils? i'm not sure about this 
UTILS = \

SRCS = $(addprefix $(SRC_DIR)/core/, $(CORE)) \
	$(addprefix $(SRC_DIR)/cgi/, $(CGI)) \
	$(addprefix $(SRC_DIR)/config/, $(CONFIG)) \
	$(addprefix $(SRC_DIR)/response/, $(HTTPRESPONSE)) \
	$(addprefix $(SRC_DIR)/request/, $(HTTPREQUEST)) \
	$(addprefix $(SRC_DIR)/network/, $(NETWORK)) \
	$(addprefix $(SRC_DIR)/utils/, $(UTILS))


OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p "$(dir $@)"
	@$(CPP) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(RMDIR) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

debug: CPPFLAGS += $(DEBUG_FLAGS)
debug: re

-include $(DEPS)

.PHONY: all clean fclean re debug
