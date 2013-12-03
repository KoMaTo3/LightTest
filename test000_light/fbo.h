#pragma once

#include <vector>

#include "windows.h"
#include "gl/gl.h"
#include "gl/gl3.h"

class Fbo
{
public:
  Fbo( int setWidth, int setHeight );
  virtual ~Fbo();
  void Bind();
  void Unbind();
  void Draw();
  void BindTexture( unsigned int levelNum );
  void UnbindTexture();
  void AddTexture( GLint components, GLenum format, GLenum type );

  std::vector< GLuint > textures;
  std::vector< GLenum > drawBuffersParameters;
  GLuint  frameBufferObject;
  int     width,
          height;

};
