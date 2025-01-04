
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include "Common.hpp"




SDL_GPUShader* LoadShader(
    SDL_GPUDevice* gpuDevice,
    const char* shaderName
);

int Draw(Context* context, SDL_GPUGraphicsPipeline* FillPipeline);


int DrawWithVertexBuffer(Context* context, SDL_GPUGraphicsPipeline* FillPipeline);