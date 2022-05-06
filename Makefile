
# https://stackoverflow.com/a/1951111

.PHONY: run clean all

all: build build/aqp

build/aqp: main.cpp
	c++ -std=c++20 -o build/aqp main.cpp

build:
	@mkdir -p build

run: all
	./build/aqp

clean:
	rm -r build/*

