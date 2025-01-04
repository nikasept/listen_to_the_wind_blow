#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include "Graphics.hpp"


/* USEFUL STRUCTS
(1) SDL_GPUComputePipeline
(2) SDL_GPUComputePipelineCreateInfo
(3) SDL_GPUShader
(4) SDL_GPUDevice
(6) SDL_GPUShaderStage
(7) SDL_GPUShaderFormat
(8) SDL_GPUShaderCreateInfo
(9) SDL_GPUGraphicsPipelineCreateInfo
(10) SDL_GPUCommandBuffer
(11) SDL_GPUViewPort
*/

/* USEFUL METHODS
(1) SDL_CreateWindow(const char *title, int w, int h, SDL_WindowFlags flags)
(2) SDL_ClaimWindowForGPUDevice(SDL_GPUDevice *device, SDL_Window *window)
(3) SDL_ReleaseWindowFromGPUDevice(SDL_GPUDevice *device, SDL_Window *window)
(4) SDL_DestroyWindow(SDL_Window *window)
(5) SDL_DestroyGPUDevice(SDL_GPUDevice *device)
(6) SDL_CreateGPUShader(SDL_GPUDevice *device, const SDL_GPUShaderCreateInfo *createinfo)
(7) SDL_AcquireGPUCommandBuffer(SDL_GPUDevice *device)
(8) SDL_CreateGPUDevice(SDL_GPUShaderFormat format_flags, bool debug_mode, const char *name)
(9) SDL_ReleaseGPUShader(SDL_GPUDevice *device, SDL_GPUShader *shader)
(10) SDL_CreateGPUGraphicsPipeline(SDL_GPUDevice *device, const SDL_GPUGraphicsPipelineCreateInfo *createinfo)
(11) SDL_GetGPUSwapchainTextureFormat(SDL_GPUDevice *device, SDL_Window *window)
*/


/*
Task 1: Make the window appear [X]
Task 2: Make shaders [X]
Task 3: Load the shaders [X]
Task 4: Make Graphics Pipeline [X]
Task 5: Learn how to input vertices to the shader [X]
Task 6: Make a Fragment Shader [X]
Task 7: Draw a first triangle [X]

Task7: What is a swapchain? [~]

Unforseen Tasks:
(1) Installed vulkan-radeon (mesa project) 
(2) glslc compilation required version 450
*/



static SDL_GPUGraphicsPipeline* FillPipeline;
static void init(Context* context) {
    
    bool deviceCreation = SDL_ClaimWindowForGPUDevice(context->gpuDevice, context->window);

    assert(deviceCreation);

    SDL_GPUShader* vertexShader = LoadShader(context->gpuDevice, "triangle.vert.spv");
    SDL_GPUShader* fragmentShader = LoadShader(context->gpuDevice, "triangle.frag.spv");

    SDL_GPUGraphicsPipelineCreateInfo info = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = {
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
                .format = SDL_GetGPUSwapchainTextureFormat(context->gpuDevice, context->window)
            }},
            .num_color_targets = 1,
        },     
        .rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL,
    };
    
    FillPipeline = SDL_CreateGPUGraphicsPipeline(context->gpuDevice,&info);
    assert(FillPipeline != NULL);

    SDL_ReleaseGPUShader(context->gpuDevice, vertexShader);
    SDL_ReleaseGPUShader(context->gpuDevice, fragmentShader);

}

static void destroy(Context* context) {
    SDL_ReleaseGPUGraphicsPipeline(context->gpuDevice, FillPipeline);
    SDL_ReleaseWindowFromGPUDevice(context->gpuDevice,  context->window);
    SDL_DestroyWindow(context->window);
    SDL_DestroyGPUDevice(context->gpuDevice);
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    // Lacunas are tracked gaps of knowledge
    Context context = Context {
        .window = SDL_CreateWindow("Title", 900, 800, 0),
        // Lacuna 1: we don't know flags
        .gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL)
        };

    init(&context);
    Draw(&context, FillPipeline);

    SDL_Event e;
    bool quit = false;
    while(!quit){
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        SDL_Delay(1000/60);
    }


    destroy(&context);
    
}
