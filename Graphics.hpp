
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include "Common.hpp"
#include "DrawablePrimitive.hpp"


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

bool DrawPrimitive(DrawablePrimitive* primitive);

bool BeginDrawing(Context *context, SDL_FColor clearColor);
bool PresentAndStopDrawing();

SDL_GPUBuffer* CreateGPUTriangeVertexBuffer(Context* context,
					    PositionColorVertex vertex[3]);

DrawablePrimitive* CreateGPUQuadPrimitive(Context* context,
					  SDL_GPUGraphicsPipeline* pipeline,
					  SDL_GPUTexture* texture,
					  PositionColorVertex vertex[4]);

DrawablePrimitive* CreateGPUTrianglePrimitive(Context* context,
					  SDL_GPUGraphicsPipeline* pipeline,
					  SDL_GPUTexture* texture,
					  PositionColorVertex vertex[3]);
#endif
