#include <SDL3/SDL_gpu.h>

#ifndef COMMON_HPP
#define COMMON_HPP


class Context {
public:
    SDL_Window* window;
    SDL_GPUDevice* gpuDevice;
};

class PositionColorVertex {
public:
    float x, y, z;
    Uint8 r, g, b, a;
};

#endif
