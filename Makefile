# Simple build without CMake:  make        -> ./adventure_game
#                              make run    -> build and start the game
CXX      ?= clang++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2
INCLUDES  = -Iinclude/core -Iinclude/entities -Iinclude/recording -Iinclude/utils
SOURCES  := $(shell find src -name '*.cpp')

adventure_game: $(SOURCES) $(shell find include -name '*.h')
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $@

run: adventure_game
	./adventure_game

clean:
	rm -rf adventure_game adventure_game.dSYM

.PHONY: run clean
