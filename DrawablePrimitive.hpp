
#ifndef DRAWABLEPRIMITIVE_HPP
#define DRAWABLEPRIMITIVE_HPP

#include <SDL3/SDL.h>
#include "Common.hpp"

class DrawablePrimitive {
public:
  DrawablePrimitive(Context* _context,
		    SDL_GPUBuffer* _vertex_buf,
		    SDL_GPUBuffer* _index_buf,
		    SDL_GPUGraphicsPipeline* _pipeline,
		    SDL_GPUTexture* _texture);
  
  ~DrawablePrimitive();

  bool hasTexture() const;
  bool hasIndexBuffer() const;
  Context* getContext() const;
  SDL_GPUBuffer* getVertexBuffer() const;
  SDL_GPUBuffer* getIndexBuffer() const;
  SDL_GPUGraphicsPipeline* getPipeline() const;
  SDL_GPUTexture* getTexture() const;

private:
  Context* context;
  SDL_GPUBuffer* vertexBuf;
  SDL_GPUBuffer* indexBuf;
  SDL_GPUGraphicsPipeline* pipeline;
  SDL_GPUTexture* texture;
};

#endif

