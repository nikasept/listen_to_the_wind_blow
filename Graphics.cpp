#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include <iostream>
#include "Common.hpp"

SDL_GPUShader* LoadShader(
    SDL_GPUDevice* gpuDevice,
    const char* shaderName
) {
    SDL_GPUShaderStage shaderStage;

    if(SDL_strstr(shaderName, ".vert")){
        shaderStage = SDL_GPU_SHADERSTAGE_VERTEX;
    }else {
        shaderStage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    }


    char fullPath[256];  
    SDL_GPUShaderFormat backendFormat = SDL_GetGPUShaderFormats(gpuDevice); 
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

    if(backendFormat & SDL_GPU_SHADERFORMAT_SPIRV) {
        // !This might cause erros to if something goes wrong test this!
        const char* BasePath = SDL_GetBasePath(); 
        SDL_snprintf(fullPath, sizeof(fullPath), "%s/shaders/compiled/%s", BasePath, shaderName);
        format = backendFormat;
    } else {
        std::cerr << "Unrecognizable gpu shader format" << std::endl;
        return NULL;
    }


    size_t codeSize;
    void* code = SDL_LoadFile(fullPath, &codeSize); 

    assert(code != NULL);   

    SDL_GPUShaderCreateInfo info = {
        .code_size = codeSize,
        .code = (Uint8*)code,
        .entrypoint = "main",
        .format = format,
        .stage = shaderStage,
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0

    };

    SDL_GPUShader* shader = SDL_CreateGPUShader(gpuDevice, &info);

    assert(shader != NULL);

    SDL_free(code);
    return shader;

}


int Draw(Context *context, SDL_GPUGraphicsPipeline* FillPipeline){
    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(context->gpuDevice);

    assert(cmdBuffer != NULL);


    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuffer, context->window, &swapchainTexture, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return -1;
    }

	if (swapchainTexture != NULL)
	{
		SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
		colorTargetInfo.texture = swapchainTexture;
		colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
		colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

		SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, NULL);
		SDL_BindGPUGraphicsPipeline(renderPass, FillPipeline);
		
		SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
		SDL_EndGPURenderPass(renderPass);
	}

	SDL_SubmitGPUCommandBuffer(cmdBuffer);

	return 0;
}

int DrawWithVertexBuffer(Context *context, SDL_GPUGraphicsPipeline *FillPipeline, SDL_GPUBuffer* VertexBuffer){

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(context->gpuDevice);

    SDL_GPUTexture* swapchainTexture; 

    bool swapchainTextureReceived = SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuffer, context->window, &swapchainTexture, 0, 0);

    assert(swapchainTexture);

    if(swapchainTexture != NULL) {

		SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
		colorTargetInfo.texture = swapchainTexture;
		colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
		colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, NULL); 


        SDL_BindGPUGraphicsPipeline(renderPass, FillPipeline); 


        SDL_GPUBufferBinding bufferBinding = (SDL_GPUBufferBinding){ .buffer = VertexBuffer, .offset = 0 };
        SDL_BindGPUVertexBuffers(renderPass, 0, &bufferBinding, 1);
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass);

    }

    SDL_SubmitGPUCommandBuffer(cmdBuffer);

    return 0;
}

