CC = g++
CFLAGS = -Wall -std=c++14

CLIENT_SRC = src/client.cpp
SERVER_SRC = src/server.cpp

SERVER_BIN = bin/server
CLIENT_BIN = bin/client

.PHONY: all clean

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $<

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)