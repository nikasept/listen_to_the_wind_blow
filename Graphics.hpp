
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include "Common.hpp"

#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

class RenderPassInfo {
public:
    SDL_GPUCommandBuffer* cmdBuffer;
    SDL_GPUTexture* swapchainTexture;
    SDL_GPUColorTargetInfo* colorTargetInfo;
    SDL_GPURenderPass* renderPass;
};



SDL_GPUShader* LoadShader(
    SDL_GPUDevice* gpuDevice,
    const char* shaderName
);

int DrawShader(Context* context, SDL_GPUGraphicsPipeline* FillPipeline);


int DrawWithVertexBuffer(Context* context, SDL_GPUGraphicsPipeline* FillPipeline, SDL_GPUBuffer* VertexBuffer);

bool BeginDrawing(Context *context, SDL_FColor clearColor);
bool PresentAndStopDrawing();
#endif
