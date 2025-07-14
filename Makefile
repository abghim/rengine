all: rengine
rengine: core.cpp
	clang++ core.cpp -o rengine -Wno-deprecated-declarations 
