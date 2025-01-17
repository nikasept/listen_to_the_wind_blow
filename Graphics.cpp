
#include <SDL3/SDL.h>
#include <cassert>
#include <cstring>
#include <iostream>

#include "Common.hpp"
#include "DrawablePrimitive.hpp"
#include "Graphics.hpp"

bool isDrawing = false;
RenderPassInfo* renderPassInfo;

SDL_GPUShader* LoadShader(
    SDL_GPUDevice* gpuDevice,
    const char* shaderName
) {
    SDL_GPUShaderStage shaderStage;

    bool isFrag = false;
    if(SDL_strstr(shaderName, ".vert")){
        shaderStage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else {
        shaderStage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	isFrag = true;
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

    // TODO: use a shader reflecter
    // this is too hard coded.
    // vulkan SPRV-reflecter on github looks nice.

    SDL_GPUShaderCreateInfo info;
    std::cout<<"creating shader----"<<std::endl;
    
    if (isFrag) {
      std::cout<<"frag"<<std::endl;
      info = {
	.code_size = codeSize,
	.code = (Uint8*)code,
	.entrypoint = "main",
	.format = format,
	.stage = shaderStage,
	.num_samplers = 1,
	.num_storage_textures = 0,
	.num_storage_buffers = 0,
	.num_uniform_buffers = 0
	
      };
    }
    else {
      std::cout<<"vertex"<<std::endl;
      info = {
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
    }

    SDL_GPUShader* shader = SDL_CreateGPUShader(gpuDevice, &info);

    assert(shader != NULL);

    SDL_free(code);
    return shader;

}

// depricated, only use of one pipline did this.
// one pipeline thus one shader.
int DrawOnlyShader(Context *context, SDL_GPUGraphicsPipeline* FillPipeline) {
    SDL_GPUCommandBuffer* cmdBuffer =
      SDL_AcquireGPUCommandBuffer(context->gpuDevice);

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
    assert(renderPassInfo->colorTargetInfo);
}

bool PresentAndStopDrawing() {
  //testRenderPassInfo();

    // no need to free the swapchaintexture.
    SDL_EndGPURenderPass(renderPassInfo->renderPass);
    SDL_SubmitGPUCommandBuffer(renderPassInfo->cmdBuffer);

    //free, use deconstructor for this??
    SDL_free(renderPassInfo->colorTargetInfo);
    delete renderPassInfo;
    return true;
}

int DrawWithVertexBuffer(Context *context,
			 SDL_GPUGraphicsPipeline* FillPipeline,
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

bool DrawPrimitive(DrawablePrimitive* primitive) {

    if(renderPassInfo->swapchainTexture == NULL) {
        std::cout << "no swapchain texture" << std::endl;
	return -1;
    }

    SDL_GPURenderPass* renderPass = renderPassInfo->renderPass;

    // pipeline here, could do the pass multiple times.
    SDL_BindGPUGraphicsPipeline(renderPass,
				primitive->getPipeline()); 

    
    SDL_GPUBufferBinding vertexBinding =
      (SDL_GPUBufferBinding){ .buffer = primitive->getVertexBuffer(),
			      .offset = 0 };

    SDL_GPUBufferBinding indexBinding =
      (SDL_GPUBufferBinding){ .buffer = primitive->getIndexBuffer(),
			      .offset = 0 };

    SDL_BindGPUVertexBuffers(renderPass,
			     0, &vertexBinding, 1);
   
    if (primitive->hasIndexBuffer()) {
        SDL_BindGPUIndexBuffer(renderPass,
			       &indexBinding,
			       SDL_GPU_INDEXELEMENTSIZE_16BIT);
	
    if (primitive->hasTexture()) {
      SDL_GPUTextureSamplerBinding samplerTextureBind = {
	.texture = primitive->getTexture(),
	.sampler = primitive->getSampler()
      };

      SDL_BindGPUFragmentSamplers(renderPass, 0, &samplerTextureBind, 1);
    }
 
	SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
    }
    else {
        // for regular triangles 
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    }

    return 0;
}




SDL_GPUGraphicsPipeline* CreateBasicFillPipeline(Context* context,
						 SDL_GPUShader* vertShader,
						 SDL_GPUShader* fragShader){

    // no textures here. 

    SDL_GPUGraphicsPipelineCreateInfo info = {
        .vertex_shader = vertShader,
        .fragment_shader = fragShader, 
        .vertex_input_state = (SDL_GPUVertexInputState){
	  .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[])
	  {{
                .slot = 0,
                .pitch = sizeof(PositionColorVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0
            }},
            .num_vertex_buffers = 1,
           
            .vertex_attributes = (SDL_GPUVertexAttribute[]){
                {
                    .location = 0,
                    .buffer_slot = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                    .offset = 0,
                },
                {
                    .location = 1,
                    .buffer_slot = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
                    .offset = sizeof(float) * 3
                }
            },

            .num_vertex_attributes = 2
        },
        .target_info = {
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]) {
            { 
                    .format =
		      SDL_GetGPUSwapchainTextureFormat(context->gpuDevice,
						       context->window)
                }
            },
            .num_color_targets = 1,
        },
    };

    info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

    return SDL_CreateGPUGraphicsPipeline(context->gpuDevice, &info); 
}


SDL_GPUTexture* CreateTexture(Context* ctx, SDL_Surface* surface) {
    SDL_GPUTexture* texture;
    SDL_GPUTextureCreateInfo textureCreateInfo = {
      .type = SDL_GPU_TEXTURETYPE_2D,
      .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
      .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
      .width = (Uint32)surface->w,
      .height = (Uint32)surface->h,
      .layer_count_or_depth = 1,
      .num_levels = 1,
    };

    texture = SDL_CreateGPUTexture(ctx->gpuDevice, &textureCreateInfo);

    SDL_SetGPUTextureName(ctx->gpuDevice, texture, "my balls");
    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = (Uint32)surface->w * (Uint32)surface->h * 4 // r g b a
    };
    
    SDL_GPUTransferBuffer* textureTransferBuffer =
      SDL_CreateGPUTransferBuffer(ctx->gpuDevice, &transferBufferInfo);

    Uint8* textureTransferPtr =
      (Uint8*)SDL_MapGPUTransferBuffer(ctx->gpuDevice,
				       textureTransferBuffer, false);
			 
    std::cout<<"bouta mem copy!!!!"<<std::endl;
    //SDL_memcpy(textureTransferPtr, surface->pixels,
    //	       surface->w * surface->h * 4);
    std::memcpy(textureTransferPtr, surface->pixels,
		surface->w * surface->h * 4);

    std::cout<<"memcopy done"<<std::endl;
    SDL_UnmapGPUTransferBuffer(ctx->gpuDevice, textureTransferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf =
      SDL_AcquireGPUCommandBuffer(ctx->gpuDevice);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);


    SDL_GPUTextureTransferInfo textureTransferInfo = {
      .transfer_buffer = textureTransferBuffer,
      .offset = 0, /* Zeros out the rest, what does that mean???!? */
    };

    SDL_GPUTextureRegion textureRegion = {
      .texture = texture,
      .w = (Uint32)surface->w,
      .h = (Uint32)surface->h,
      .d = 1
    };

    SDL_UploadToGPUTexture(copyPass, &textureTransferInfo, &textureRegion,
			   false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
    SDL_ReleaseGPUTransferBuffer(ctx->gpuDevice, textureTransferBuffer);

    return texture;
}
