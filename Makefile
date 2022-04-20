
all: build build/aqp

build/aqp: main.cpp
	c++ -o build/aqp main.cpp

build:
	mkdir -p build

run: all
	./build/aqp

clean:
	rm -r build/*

