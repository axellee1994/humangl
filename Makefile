NAME    = humangl

CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

SRCS    = src/main.cpp \
          src/Model.cpp \
          src/Animation.cpp \
          src/Shader.cpp \
          src/Math.cpp \
          src/TextRenderer.cpp

OBJDIR  = obj
OBJS    = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS))

LIBS    = -lGL -lGLEW -lglfw

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LIBS)

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
