#include <SDL3/SDL_gpu.h>

#ifndef COMMON_HPP
#define COMMON_HPP


class Context {
public:
    SDL_Window* window;
    SDL_GPUDevice* gpuDevice;
};

#endif