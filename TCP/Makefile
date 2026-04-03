CXX      = g++
CXXFLAGS = -std=c++17 -Wall -pthread

LIBDIR = ../SocketLib
LIBSRC = $(LIBDIR)/socket.cpp \
         $(LIBDIR)/address.cpp \
         $(LIBDIR)/listener.cpp \
         $(LIBDIR)/tcp_socket.cpp

all: server client

server: server.cpp $(LIBSRC)
	$(CXX) $(CXXFLAGS) server.cpp $(LIBSRC) -o server

client: client.cpp $(LIBSRC)
	$(CXX) $(CXXFLAGS) client.cpp $(LIBSRC) -o client

clean:
	rm -f server client
