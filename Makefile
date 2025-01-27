files := main.cpp Graphics.cpp DrawablePrimitive.cpp \
spirv_reflect/spirv_reflect.cpp

all: 
	glslc shaders/source/basic.vert -o shaders/compiled/basic.vert.spv
	glslc shaders/source/scaledown.vert -o shaders/compiled/scaledown.vert.spv
	glslc shaders/source/textured.vert -o shaders/compiled/textured.vert.spv

	glslc shaders/source/textured.frag -o shaders/compiled/textured.frag.spv
	glslc shaders/source/basic.frag -o shaders/compiled/basic.frag.spv
	clang++ $(files) -lSDL3 -o sdl_test -std=c++20 
