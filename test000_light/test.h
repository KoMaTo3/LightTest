#ifndef __TEST_H__
#define __TEST_H__


#include "windows.h"
#include "gl/gl.h"
#include <vector>

#include "kvector.h"
#include "lightmgr.h"


class LightRenderer;
class Graphics;
class LightTest;


extern LightTest *lightTest;


class LightTest {
public:
  enum Textures {
    LIGHTTEST_TEXTURE_BACKGROUND0 = 0,
    LIGHTTEST_TEXTURE_BLOCK,
  };
  struct Camera {
    Vec2 leftTop;
    Vec2 rightBottom;
    float movementSpeed;
    Vec2 position;
    float scale;
    float scaleTarget;
  };
  LightRenderer *lightRenderer;

public:
  LightTest( int argc, char* argv[] );
  virtual ~LightTest();
  void Run();
  static void Redraw();
  void Update();
  inline const Camera& GetCamera() const { return this->camera; }
  inline const Size& GetWindowSize() const { return this->windowSize; }
  GLuint GetTexture( unsigned int id );
  static Vec2 RecalculatePosition( const Vec2& pos, const Camera& camera );

private:
  Graphics *core;
  std::vector< GLuint > texturesList;
  Size windowSize;
  LightList *lightList;
  Camera camera;
  std::string action;

private:
  LightTest();
};

#endif
