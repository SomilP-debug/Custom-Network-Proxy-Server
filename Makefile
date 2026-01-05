CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
TARGET = proxy

SRC = \
	src/main.cpp \
	src/proxy.cpp \
	src/http_parser.cpp \
	src/https_tunnel.cpp \
	src/net_utils.cpp \
	src/logger.cpp \
	src/metrics.cpp \
	src/blocklist.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
