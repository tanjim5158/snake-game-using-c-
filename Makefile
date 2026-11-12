# Base configurations
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iexternal/raylib/src
LDFLAGS = -Lexternal/raylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Executable name
TARGET = snake

# Sources
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
