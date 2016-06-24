#!/usr/bin/make -f
# Makefile - 2016 - Atlee Brink

CXX ?= g++
CXXFLAGS = -O3 -std=c++11 -Wall -Wfatal-errors
RM ?= rm

all: markpart

markpart: markpart.cpp cmark.hpp
	$(CXX) $(CXXFLAGS) -o markpart markpart.cpp

clean:
	$(RM) -f markpart
