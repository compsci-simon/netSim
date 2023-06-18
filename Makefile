CC = g++
CFLAGS = -Wall -std=c++14 -g

NODE_SRC = src/node.cpp
ROUTER_SRC = src/router.cpp

ROUTER_BIN = bin/server
NODE_BIN = bin/client

.PHONY: all clean

all: clean $(ROUTER_BIN) $(NODE_BIN)

$(ROUTER_BIN): src/server.cpp src/dhcp.cpp $(ROUTER_SRC) src/packet.cpp src/datagram.cpp src/frame.cpp src/utils.cpp src/arp.cpp
	$(CC) $(CFLAGS) -o $@ $^

$(NODE_BIN): src/client.cpp $(NODE_SRC) $(ROUTER_SRC) src/packet.cpp src/datagram.cpp src/frame.cpp src/dhcp.cpp src/utils.cpp src/arp.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(ROUTER_BIN) $(ROUTER_BIN).dSYM $(NODE_BIN) $(NODE_BIN).dSYM bin/frame.dSYM bin/node.dSYM bin/router.dSYM /Users/simon/Desktop/netSim.log
