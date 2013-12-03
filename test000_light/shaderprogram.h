#ifndef __SHADERPROGRAM_H__
#define __SHADERPROGRAM_H__


#include <gl/gl3.h>
#include <string>


class ShaderProgram
{
public:
  ShaderProgram( const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName );
  virtual ~ShaderProgram();
  GLint GetUniformLocation( const std::string& uniformName );
  void UseProgram();

private:
  GLuint program;
  GLuint vertex;
  GLuint fragment;

  ShaderProgram();
  ShaderProgram( ShaderProgram& );
  ShaderProgram& operator=( ShaderProgram& );
  bool _CheckShaderError( const std::string& text, GLuint shader );
};


#endif
