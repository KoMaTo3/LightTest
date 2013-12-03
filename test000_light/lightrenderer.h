#ifndef __LIGHTRENDERER_H__
#define __LIGHTRENDERER_H__


class LightMgr;
class Vec2;
class VertexArray;


#include "lightmgr.h"


class LightRenderer
{
public:
  LightRenderer(
    const Vec2& setTextureSize,
    const std::string& vertexShaderLightFileName,
    const std::string& fragmentShaderLightFileName,
    const std::string& vertexShaderResultFileName,
    const std::string& fragmentShaderResultFileName,
    LightsListPtr setLightList
    );
  virtual ~LightRenderer();
  void BeginScene();
  void EndScene();
  inline LightMgr* GetLightManager() {
    return this->manager;
  }
  void Update();
  void SetRect( const Vec2& leftTop, const Vec2& rightBottom );

private:
  LightMgr    *manager;
  Fbo         *fbo;
  VertexArray *vao;
  struct {
    ShaderProgram *shaderProgram;
    GLint
      uniformTextureScene,
      uniformTextureLight,
      uniformTextureBrightness,
      uniformOneByWindowSize;
  } shader;
  #pragma pack( push, 4 )
  struct Vertice {
    Vec2 position;
    Vec2 texCoords;
  };
  #pragma pack( pop )
  GLuint sceneVertexBuffer;

  LightRenderer( const LightRenderer& );
  LightRenderer();
};


#endif
