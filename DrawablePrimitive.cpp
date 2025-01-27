
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "Common.hpp"
#include "DrawablePrimitive.hpp"

DrawablePrimitive::DrawablePrimitive(Context* _context,
				     SDL_GPUBuffer* _vertex_buf,
				     SDL_GPUBuffer* _index_buf,
				     SDL_GPUGraphicsPipeline* _pipeline){
    vertexBuf = _vertex_buf;
    indexBuf = _index_buf;
    pipeline = _pipeline;
    context = _context;
    textureBinds = {};

}

DrawablePrimitive::~DrawablePrimitive() {
    SDL_ReleaseGPUBuffer(context->gpuDevice, vertexBuf);

    if (indexBuf != NULL) {
      SDL_ReleaseGPUBuffer(context->gpuDevice, indexBuf);
    }
}


bool DrawablePrimitive::hasTexture() const {
    return (textureBinds.size() > 0);
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

std::vector<SDL_GPUTextureSamplerBinding>*
DrawablePrimitive::getTextureBinds(){
    return &textureBinds;
}

void DrawablePrimitive::addTexture(SDL_GPUTexture* tex,
				   SDL_GPUSampler* sampl) {
  SDL_GPUTextureSamplerBinding bind = {
    .texture = tex,
    .sampler = sampl
  };

  textureBinds.push_back(bind);
}

SDL_GPUBuffer* CreateGPUTriangeVertexBuffer(Context* context,
					    PositionColorVertex vertex[3]) {
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo  = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
	.size = sizeof(PositionColorVertex) * 3,
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
      .buffer = vertexBuffer,
      .offset = 0,
      .size = sizeof(PositionColorVertex) * 3
    }; 

    // Why did we map if we were going to specify information about source and destination again?
    SDL_UploadToGPUBuffer(pass, &source, &destination, false);



    SDL_EndGPUCopyPass(pass);
    SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
    SDL_ReleaseGPUTransferBuffer(context->gpuDevice, transferBuffer);

    return vertexBuffer;
}

DrawablePrimitive* CreateGPUTrianglePrimitive(Context* context,
					  SDL_GPUGraphicsPipeline* pipeline,
					  PositionColorVertex vertex[3]) {
  SDL_GPUBuffer* triangleVertex =
    CreateGPUTriangeVertexBuffer(context, vertex);

  DrawablePrimitive* primitive =
    new DrawablePrimitive(context, triangleVertex, NULL, pipeline);

  return primitive;
}
 

DrawablePrimitive* CreateGPUQuadPrimitive(Context* context,
					  SDL_GPUGraphicsPipeline* pipeline,
					  PositionColorVertex vertex[4]) {
  //initialize buffers
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo  = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
	.size = sizeof(PositionColorVertex) * 4,
    };

    SDL_GPUBufferCreateInfo indexBufferCreateInfo  = {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
	.size = sizeof(Uint16) * 6,
    };
    
    SDL_GPUBuffer* vertexBuffer;

    vertexBuffer = SDL_CreateGPUBuffer(
        context->gpuDevice,
        &vertexBufferCreateInfo
    ); 

    SDL_SetGPUBufferName(context->gpuDevice, vertexBuffer, "quadBuffer");

    SDL_GPUBuffer* indexBuffer;

    indexBuffer = SDL_CreateGPUBuffer(
        context->gpuDevice,
        &indexBufferCreateInfo
    ); 

    // get ready to transfer, (both)

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = (sizeof(PositionColorVertex) * 4) + (sizeof(Uint16) * 6)
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
    Uint16* indexData = (Uint16*) &transferData[4];
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

    // this is transfer bit.
    // each one needs its own transfer buffer and uploade

    // vertex
    SDL_GPUTransferBufferLocation vertexSrc =
      SDL_GPUTransferBufferLocation {.transfer_buffer = transferBuffer,
				     .offset = 0 };
    SDL_GPUBufferRegion vertexDst =
      SDL_GPUBufferRegion{
      .buffer = vertexBuffer,
      .offset = 0,
      .size = sizeof(PositionColorVertex) * 4,
    }; 

    // index
    SDL_GPUTransferBufferLocation indexSrc = {
      .transfer_buffer = transferBuffer,
      .offset = sizeof(PositionColorVertex) * 4
    };

    SDL_GPUBufferRegion indexDst = {
      .buffer = indexBuffer,
      .offset = 0,
      .size = sizeof(Uint16) * 6,
    }; 


    // upload both.
    SDL_UploadToGPUBuffer(pass, &vertexSrc, &vertexDst, false);
    SDL_UploadToGPUBuffer(pass, &indexSrc, &indexDst, false);

    SDL_EndGPUCopyPass(pass);
    SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
    SDL_ReleaseGPUTransferBuffer(context->gpuDevice, transferBuffer);

    DrawablePrimitive* primitive = new DrawablePrimitive(context,
							 vertexBuffer,
							 indexBuffer,
							 pipeline);
    return primitive;
}

