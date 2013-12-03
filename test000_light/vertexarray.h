#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__


#include "gl/gl3.h"
#include "interngl.h"


class VertexArray
{
public:
  VertexArray();
  VertexArray( void *data, int dataLength );
  virtual ~VertexArray();
  void Bind();
  void UnBind();
  void SetData( void *data, int dataLength );

private:
  VertexArray( const VertexArray& );
  VertexArray& operator=( const VertexArray& );

  GLuint object;
  GLuint buffer;
};

#endif
