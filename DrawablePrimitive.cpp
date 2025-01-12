
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "Common.hpp"
#include "DrawablePrimitive.hpp"

DrawablePrimitive::DrawablePrimitive(Context* _context,
				     SDL_GPUBuffer* _vertex_buf,
				     SDL_GPUBuffer* _index_buf,
				     SDL_GPUGraphicsPipeline* _pipeline,
				     SDL_GPUTexture* _texture) {
    vertexBuf = _vertex_buf;
    indexBuf = _index_buf;
    pipeline = _pipeline;
    texture = _texture;
    context = _context;
}

DrawablePrimitive::~DrawablePrimitive() {
    SDL_ReleaseGPUBuffer(context->gpuDevice, vertexBuf);
    SDL_ReleaseGPUBuffer(context->gpuDevice, vertexBuf);
}


bool DrawablePrimitive::hasTexture() const {
    return (texture != NULL);
}

bool DrawablePrimitive::hasIndexBuffer() const {
    return (indexBuf != NULL);
}

Context* DrawablePrimitive::getContext() const {
    return context;
}

SDL_GPUBuffer* DrawablePrimitive::getVertexBuffer() const {
    return vertexBuf;
}

SDL_GPUBuffer* DrawablePrimitive::getIndexBuffer() const {
    return indexBuf;
}

SDL_GPUGraphicsPipeline* DrawablePrimitive::getPipeline() const {
    return pipeline;
}

SDL_GPUTexture* DrawablePrimitive::getTexture() const {
    return texture;
}


