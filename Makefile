all: rengine
core.o: core.cpp core.hpp
	clang++ -c core.cpp -Wno-deprecated-declarations

main.o: main.cpp core.hpp core.cpp
	clang++ $$(sdl2-config --cflags --libs) -c main.cpp

rengine: main.o core.o
	clang++ $$(sdl2-config --cflags --libs) main.o core.o -o rengine
