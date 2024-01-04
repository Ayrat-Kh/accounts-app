# This is the NMake shim for Windows (WIP)
# Example: $Env:WITH_ZLIB='0'; $ENV:WITH_LTO='0'; $Env:CC='clang'; $ENV:CFLAGS='-I C:\vcpkg\installed\x64-windows\include'; $ENV:LDFLAGS='-L C:\vcpkg\installed\x64-windows\lib'; $ENV:CXX='clang++'; $ENV:EXEC_SUFFIX='.exe'; $ENV:WITH_LIBUV='1'; nmake


# char *CXXFLAGS = strcpy(calloc(1024, 1), maybe(getenv("CXXFLAGS")));
# char *CFLAGS = strcpy(calloc(1024, 1), maybe(getenv("CFLAGS")));
# char *LDFLAGS = strcpy(calloc(1024, 1), maybe(getenv("LDFLAGS")));
CC=cc
CXX=g++
CXXFLAGS=-march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++20 -IuWebSockets/uSockets/src -IuWebSockets/src -Isrc
LDFLAGS=uWebSockets/uSockets/*.o -lz

DESTDIR ?=
prefix ?= /usr/local

app: default
	mkdir -p build
	$(CXX) $(CXXFLAGS) -flto src/App.cpp $(LDFLAGS) -o build/App

clean: default
	./build clean

all: default

default:
	$(MAKE) -C uWebSockets/uSockets
