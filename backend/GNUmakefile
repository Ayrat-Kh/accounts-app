
CC=cc
CXX=g++
CXXFLAGS=-march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++20 \
-Ilib/uWebSockets/uSockets/src \
-Ilib/uWebSockets/src \
-Isrc

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
