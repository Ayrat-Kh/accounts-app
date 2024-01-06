
CC=cc
CXX=g++


UWEB_SOCKET_INCLUDE=-Ilib/uWebSockets/uSockets/src -Ilib/uWebSockets/src
POCO_INCLUDE=-Ilib/poco/JWT

CXXFLAGS=-march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++20 \
${UWEB_SOCKET_INCLUDE} ${POCO_INCLUDE}

LDFLAGS=lib/uWebSockets/uSockets/*.o -lz

DESTDIR ?=
prefix ?= /usr/local

app: default
	mkdir -p build
	$(CXX) $(CXXFLAGS) -flto src/App.cpp $(LDFLAGS) -o build/App

clean: default
	./build clean

all: default

default:
	$(MAKE) -C lib/uWebSockets/uSockets
