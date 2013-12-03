#ifndef __LIGHTMAP_H__
#define __LIGHTMAP_H__


#include "windows.h"
#include "gl/gl.h"
#include "gl/gl3.h"


#include <vector>
#include <deque>
#include "lbuffer.h"


namespace LightMap {

//типы источников света
enum LightType {
  LT_POINT,
};


float GetDegreeOfPoint( const Vec2& point );


//вертексты для отрисовки света
struct LightVertex {
  Vec2 position;
  Vec4 color;
  float alpha;

  LightVertex( const Vec2& setPosition, float setAlpha )
  :position( setPosition ), color( 1.0f, 0.0f, 0.0f, 0.0f ), alpha( setAlpha )
  {
  }

  LightVertex( const Vec2& setPosition, const Vec4& setColor, float setAlpha )
  :position( setPosition ), color( setColor ), alpha( setAlpha )
  {
  }
};


//интерфейс для работы с блоками, препятствующими освещению
class ILightBlock: public ILBufferProjectedObject {
public:
  void AddEdgeToBuffer( const Vec2& lightPosition, LBuffer *buffer, const Vec2& point0, const Vec2& point1, LBufferCacheEntity *cache );
  virtual const Vec2& GetPosition() const = NULL;         //center
  virtual const Vec2& GetSize() const = NULL;             //AABB
  virtual inline const Vec2& GetHalfSize() const = NULL;  //AABB/2
  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) = 0;
  virtual void SetPosition( const Vec2& setPosition ) = 0;
  virtual void Update() = 0;
  virtual void Redraw() = 0;
};
typedef std::deque< LightMap::ILightBlock* > LightBlocksList;



class LightEntity {
public:
  Vec2 position;
  Vec4 color;
  LightType type;
  Vec2 halfSize;
  float brightness;
  LBuffer lbuffer;
  static const Vec2 vecAxis;
  static const int minimalEngesCountPerLight; //минимальное число вершин для корректной отрисовки окружности
  bool lastCheckInRect;
  float epsilon;
  float maxRange;
  float penetration;  //величина проникновения света в объекты

  std::vector< LightVertex > vertices;
  std::vector< LightVertex > verticesSoftEdges;

  LightEntity( const LightType &setType, const Vec2& setPosition, const Vec4& setColor, const Vec2& setSize, float setBrightness, int lBufferSize = 1024 * 2, float setEpsilon = 1.0f );
  virtual ~LightEntity();

  void Set( const LightType &setType, const Vec2& setPosition, const Vec4& setColor, const Vec2& setSize, float setBrightness );
  void SetSize( const Vec2& setSize );
  void SetColor( const Vec4& setColor );
  void SetPosition( const Vec2& setPosition );
  inline void SetPenetration( const float newPenetration ) { this->penetration = newPenetration; }
  inline float GetPenetration() { return this->penetration; }
  LightEntity* Clone();
  void AddVertice( const LightVertex& vertex );
  bool CheckBlockDistance( const Vec2& blockPos, const Vec2& blockHalfSize );
  bool CheckInRect( const Vec2& leftTop, const Vec2& rightBottom );
  inline bool CheckInRectPassed() {
    return this->lastCheckInRect;
  }
  void Update( LightBlocksList &blocks );
  bool IsChanged();
  inline const Vec2& GetSize() {
    return this->size;
  }
  inline const Vec4& GetColor() {
    return this->color;
  }

private:
  LightEntity();
  LightEntity( const LightEntity& );
  LightEntity& operator=( const LightEntity& );

  Vec2 size;
  Vec2 _lastPosition;
  Vec2 _lastSize;
  Vec4 _lastColor;
};


};


#endif
