
#include <SDL3/SDL.h>
#include <cassert>
#include <cstring>
#include <iostream>

#include "Graphics.hpp"
#include "Common.hpp"

bool isDrawing = false;
RenderPassInfo* renderPassInfo;

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
        SDL_snprintf(fullPath, sizeof(fullPath),
		     "%s/shaders/compiled/%s", BasePath, shaderName);
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

// depricated, only use of one pipline did this.
// one pipeline thus one shader.
int DrawShader(Context *context, SDL_GPUGraphicsPipeline* FillPipeline){
    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(context->gpuDevice);

    assert(cmdBuffer != NULL);


    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuffer, context->window, &swapchainTexture, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return -1;
    }

    if (swapchainTexture != NULL) {
      return -1;
    }
    SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.clear_color =
      (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* renderPass =
      SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo,
			     1, NULL);

    SDL_BindGPUGraphicsPipeline(renderPass, FillPipeline);
		
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(renderPass);

    SDL_SubmitGPUCommandBuffer(cmdBuffer);

    return 0;
}

bool BeginDrawing(Context *context, SDL_FColor clearColor) {
    isDrawing = true;

    SDL_GPUCommandBuffer* cmdBuffer =
       SDL_AcquireGPUCommandBuffer(context->gpuDevice);

    SDL_GPUTexture* swapchainTexture; 

    bool swapchainTextureReceived =
      SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuffer, context->window,
					    &swapchainTexture, 0, 0);

    assert(swapchainTextureReceived);

    SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.clear_color = clearColor;
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    // allocate to memory;
    SDL_GPUColorTargetInfo* pColorTargetInfo = new SDL_GPUColorTargetInfo;
    std::memcpy(pColorTargetInfo, &colorTargetInfo,
		sizeof(SDL_GPUColorTargetInfo));

    SDL_GPURenderPass* renderPass =
      SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, NULL); 

    renderPassInfo = new RenderPassInfo {
      cmdBuffer,
      swapchainTexture,
      pColorTargetInfo,
      renderPass
    };

    return true;
}

void testRenderPassInfo() {
    assert(renderPassInfo->cmdBuffer);
    assert(renderPassInfo->swapchainTexture);
    assert(renderPassInfo->renderPass);
}

bool PresentAndStopDrawing() {
    assert(renderPassInfo->cmdBuffer);
    assert(renderPassInfo->swapchainTexture);
    assert(renderPassInfo->renderPass);
    assert(renderPassInfo->colorTargetInfo);

    // no need to free the swapchaintexture.
    SDL_EndGPURenderPass(renderPassInfo->renderPass);
    SDL_SubmitGPUCommandBuffer(renderPassInfo->cmdBuffer);

    //free, use deconstructor for this??
    SDL_free(renderPassInfo->colorTargetInfo);
    delete renderPassInfo;
    return true;
}

int DrawWithVertexBuffer(Context *context,
			 SDL_GPUGraphicsPipeline *FillPipeline,
			 SDL_GPUBuffer* VertexBuffer){

    testRenderPassInfo();

    if(renderPassInfo->swapchainTexture == NULL) {
        std::cout << "no swapchain texture" << std::endl;
	return -1;
    }
    SDL_GPURenderPass* renderPass = renderPassInfo->renderPass;

    SDL_BindGPUGraphicsPipeline(renderPass,
				FillPipeline); 

    SDL_GPUBufferBinding bufferBinding =
      (SDL_GPUBufferBinding){ .buffer = VertexBuffer, .offset = 0 };

    SDL_BindGPUVertexBuffers(renderPass,
			     0, &bufferBinding, 1);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

    return 0;
}


SDL_GPUBuffer* CreateGPUTriangeVertexBuffer(Context* context,
					    PositionColorVertex vertex[3]) {
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo  = {
        .size = sizeof(PositionColorVertex) * 3,
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX
    };
    
    SDL_GPUBuffer* vertexBuffer;

    vertexBuffer = SDL_CreateGPUBuffer(
        context->gpuDevice,
        &vertexBufferCreateInfo
    ); 

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(PositionColorVertex) * 3
    };

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(
        context->gpuDevice,
        &transferBufferCreateInfo);

    PositionColorVertex* transferData = (PositionColorVertex*)SDL_MapGPUTransferBuffer(context->gpuDevice, transferBuffer, false); 

    //transferData[0] = (PositionColorVertex) {    -1,    -1, 0, 255,   0,   0, 255 };
    transferData[0] = vertex[0];
    transferData[1] = vertex[1];
    transferData[2] = vertex[2];


    // Lacuna: We don't understand Mapping
    SDL_UnmapGPUTransferBuffer(context->gpuDevice, transferBuffer);

    SDL_GPUCommandBuffer* uploadCommandBuffer =
      SDL_AcquireGPUCommandBuffer(context->gpuDevice);

    SDL_GPUCopyPass* pass = SDL_BeginGPUCopyPass(uploadCommandBuffer);

    SDL_GPUTransferBufferLocation source =
      SDL_GPUTransferBufferLocation{.transfer_buffer = transferBuffer, .offset = 0 };
    SDL_GPUBufferRegion destination =
      SDL_GPUBufferRegion{
      .offset = 0,
      .size = sizeof(PositionColorVertex) * 3,
      .buffer = vertexBuffer
    }; 

    // Why did we map if we were going to specify information about source and destination again?
    SDL_UploadToGPUBuffer(pass, &source, &destination, false);



    SDL_EndGPUCopyPass(pass);
    SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
    SDL_ReleaseGPUTransferBuffer(context->gpuDevice, transferBuffer);

    return vertexBuffer;
}

SDL_GPUBuffer* CreateGPUQuadVertexBuffer(Context* context,
					    PositionColorVertex vertex[4]) {
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo  = {
        .size = sizeof(PositionColorVertex) * 4,
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX
    };
    
    SDL_GPUBuffer* vertexBuffer;

    vertexBuffer = SDL_CreateGPUBuffer(
        context->gpuDevice,
        &vertexBufferCreateInfo
    ); 

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(PositionColorVertex) * 4 + (sizeof(Uint16) * 6)
    };

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(
        context->gpuDevice,
        &transferBufferCreateInfo);

    PositionColorVertex* transferData =
      (PositionColorVertex*)SDL_MapGPUTransferBuffer(context->gpuDevice,
						     transferBuffer, false);

    //transferData[0] = (PositionColorVertex) {    -1,    -1, 0, 255,   0,   0, 255 };
    transferData[0] = vertex[0];
    transferData[1] = vertex[1];
    transferData[2] = vertex[2];
    transferData[3] = vertex[3];

    // tell em how to connect, idk how this work.
    Uint16* indexData = (Uint16*) &transferData[4]; //after all the vetexes
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 0;
    indexData[4] = 2;
    indexData[5] = 3;

    

    // Lacuna: We don't understand Mapping
    SDL_UnmapGPUTransferBuffer(context->gpuDevice, transferBuffer);

    SDL_GPUCommandBuffer* uploadCommandBuffer =
      SDL_AcquireGPUCommandBuffer(context->gpuDevice);

    SDL_GPUCopyPass* pass = SDL_BeginGPUCopyPass(uploadCommandBuffer);

    SDL_GPUTransferBufferLocation source =
      SDL_GPUTransferBufferLocation{.transfer_buffer = transferBuffer, .offset = 0 };
    SDL_GPUBufferRegion destination =
      SDL_GPUBufferRegion{
      .offset = 0,
      .size = sizeof(PositionColorVertex) * 4,
      .buffer = vertexBuffer
    }; 

    // Why did we map if we were going to specify information about source and destination again?
    SDL_UploadToGPUBuffer(pass, &source, &destination, false);

    SDL_EndGPUCopyPass(pass);
    SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
    SDL_ReleaseGPUTransferBuffer(context->gpuDevice, transferBuffer);

    return vertexBuffer;
}
