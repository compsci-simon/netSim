CC = g++
CFLAGS = -Wall -std=c++14 -g

CLIENT_SRC = src/client.cpp
SERVER_SRC = src/server.cpp
HOSTS_SRC = src/hosts/*.cpp
PROTOCOLS_SRC = src/protocols/*.cpp
UTILS_SRC = src/utils/*.cpp

SERVER_BIN = bin/server
CLIENT_BIN = bin/client

.PHONY: all clean

all: clean $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC) $(HOSTS_SRC) $(PROTOCOLS_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC) $(HOSTS_SRC) $(PROTOCOLS_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(SERVER_BIN) $(SERVER_BIN).dSYM $(CLIENT_BIN) $(CLIENT_BIN).dSYM /Users/simon/Desktop/netSim.log
