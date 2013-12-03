#ifndef __LIGHTMGR_H__
#define __LIGHTMGR_H__


#include <memory>
#include "lightmap.h"
#include "fbo.h"
#include "shaderprogram.h"


typedef std::deque< LightMap::LightEntity* > LightList;
typedef LightList* LightsListPtr;


class VertexArray;


class LightMgr
{
public:
  LightMgr();
  virtual ~LightMgr();
  void Init( const Vec2& setTextureSize, ShaderProgram *setShader, LightsListPtr setLightList );
  void Update();
  void Render();
  void BindColorTexture();
  void BindBrightnessTexture();
  void SetRect( const Vec2& setLeftTop, const Vec2& setRightBottom );
  void ClearCacheByBlock( ILBufferProjectedObject *object );
  inline Fbo* GetFBO() {
    return this->fboLight;
  }
  inline const Vec2& GetOneByWindowSize() const {
    return this->oneByWindowSize;
  }

  LightsListPtr lightList;
  LightMap::LightBlocksList lightBlocks;
  Vec4 lightAmbient;

private:
  Fbo *fboLight;
  Vec2 oneByWindowSize;
  ShaderProgram *shader;
  struct {
    Vec2
      leftTop,
      rightBottom;
  } window;
  struct TExtureSize {
    int
      width,
      height;

    TExtureSize();
  } textureSize;

  LightMgr( const LightMgr& );
  LightMgr& operator=( const LightMgr& );

  struct {
    GLint
      textureColor,
      textureBrightness;
  } _shaderUniforms;

  static Vec3 _ambientLightVertices[ 4 ];
  std::vector< VertexArray* > vao;
};

#endif
