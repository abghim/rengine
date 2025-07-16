all: rengine
rengine: core.cpp core.hpp
	clang++ core.cpp -o rengine -Wno-deprecated-declarations 
