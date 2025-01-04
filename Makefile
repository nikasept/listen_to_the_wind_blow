files := main.cpp Graphics.cpp

all: 
	glslc shaders/source/triangle.vert -o shaders/compiled/triangle.vert.spv
	glslc shaders/source/triangle.frag -o shaders/compiled/triangle.frag.spv
	clang++ $(files) -lSDL3 -o sdl_test -std=c++20 
