CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

SRC = src/main.cpp \
      src/proxy.cpp \
      src/http_parser.cpp \
      src/net_utils.cpp \
      src/https_tunnel.cpp \
      src/logger.cpp \
      src/blocklist.cpp \
      src/metrics.cpp

OBJ = $(SRC:.cpp=.o)

proxy: $(OBJ)
	$(CXX) $(CXXFLAGS) -o proxy $(OBJ)

clean:
	rm -f proxy $(OBJ)
