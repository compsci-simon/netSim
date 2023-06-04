CC = g++
CFLAGS = -Wall -std=c++14 -g

NODE_SRC = src/node.cpp
ROUTER_SRC = src/router.cpp

ROUTER_BIN = bin/router
NODE_BIN = bin/node

.PHONY: all clean

all: $(ROUTER_BIN) $(NODE_BIN)

$(ROUTER_BIN): $(ROUTER_SRC)
	$(CC) $(CFLAGS) -o $@ $<

$(NODE_BIN): $(NODE_SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(ROUTER_BIN) $(ROUTER_BIN).dSYM $(NODE_BIN) $(NODE_BIN).dSYM