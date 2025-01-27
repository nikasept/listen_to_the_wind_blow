
#ifndef DRAWABLEPRIMITIVE_HPP
#define DRAWABLEPRIMITIVE_HPP

#include <SDL3/SDL.h>
#include <vector>
#include "Common.hpp"

class DrawablePrimitive {
public:
  DrawablePrimitive(Context* _context,
		    SDL_GPUBuffer* _vertex_buf,
		    SDL_GPUBuffer* _index_buf,
		    SDL_GPUGraphicsPipeline* _pipeline);
  
  ~DrawablePrimitive();

  bool hasTexture() const;
  bool hasIndexBuffer() const;

  Context* getContext() const;
  SDL_GPUBuffer* getVertexBuffer() const;
  SDL_GPUBuffer* getIndexBuffer() const;
  SDL_GPUGraphicsPipeline* getPipeline() const;
  std::vector<SDL_GPUTextureSamplerBinding>* getTextureBinds(); 

  void addTexture(SDL_GPUTexture* tex, SDL_GPUSampler* sampl);

private:
  Context* context;
  SDL_GPUBuffer* vertexBuf;
  SDL_GPUBuffer* indexBuf;
  SDL_GPUGraphicsPipeline* pipeline;
  std::vector<SDL_GPUTextureSamplerBinding> textureBinds;
};

SDL_GPUBuffer* CreateGPUTriangeVertexBuffer(Context* context,
					    PositionColorVertex vertex[3]);

DrawablePrimitive* CreateGPUQuadPrimitive(Context* context,
					  SDL_GPUGraphicsPipeline* pipeline,
					  PositionColorVertex vertex[4]);

DrawablePrimitive* CreateGPUTrianglePrimitive(Context* context,
					  SDL_GPUGraphicsPipeline* pipeline,
					  PositionColorVertex vertex[3]);

#endif

