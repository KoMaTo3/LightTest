#pragma once

#include "types.h"
#include "kmath.h"
#include "file.h"

class Angles;
class Mat3;
class Polar3;

class Vec2 {
public:
  Float   x;
  Float   y;

  Vec2( Void );
  explicit Vec2( const Float x, const Float y );

  Void      Set( const Float x, const Float y );
  Void      Zero( Void );

  Float     operator[]( Int index ) const;
  Float &   operator[]( Int index );
  Vec2      operator-() const;
  Float     operator*( const Vec2 &a ) const;
  Vec2      operator*( const Float a ) const;
  Vec2      operator/( const Float a ) const;
  Vec2      operator+( const Vec2 &a ) const;
  Vec2      operator-( const Vec2 &a ) const;
  Vec2 &    operator+=( const Vec2 &a );
  Vec2 &    operator-=( const Vec2 &a );
  Vec2 &    operator/=( const Vec2 &a );
  Vec2 &    operator/=( const Float a );
  Vec2 &    operator*=( const Float a );

  friend Vec2  operator*( const Float a, const Vec2 b );

  Bool      Compare( const Vec2 &a ) const;              // exact compare, no epsilon
  Bool      Compare( const Vec2 &a, const Float epsilon ) const;    // compare with epsilon
  Bool      operator==(  const Vec2 &a ) const;            // exact compare, no epsilon
  Bool      operator!=(  const Vec2 &a ) const;            // exact compare, no epsilon

  Float     Length( Void ) const;
  Float     LengthFast( Void ) const;
  Float     LengthSqr( Void ) const;
  Float     Normalize( Void );      // returns length
  Float     NormalizeFast( Void );    // returns length
  Vec2 &    Truncate( Float length );  // cap length
  Void      Clamp( const Vec2 &min, const Vec2 &max );
  Void      Snap( Void );        // snap to closest integer value
  Void      SnapInt( Void );      // snap towards integer (floor)
  Void      Rotate90CW(); //k
  Void      Rotate90CCW();  //k
  Vec2      Projection( Vec2 &a );  //k
  Float     Dot( const Vec2 &a ) const; //k
  Bool      IsLess( const Vec2 &a ) const;
  static Float  IntersectDet( Float a, Float b, Float c, Float d );
  static Bool   TestIntersect( const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, Vec2 *result, Float epsilon = 0.0001f );

  Int       GetDimension( Void ) const;

  const Float *  ToFloatPtr( Void ) const;
  Float *   ToFloatPtr( Void );

  Void      Lerp( const Vec2 &v1, const Vec2 &v2, const Float l );
};

KM_INLINE Vec2::Vec2( Void ) {
}

KM_INLINE Vec2::Vec2( const Float x, const Float y ) {
  this->x = x;
  this->y = y;
}

KM_INLINE Void Vec2::Set( const Float x, const Float y ) {
  this->x = x;
  this->y = y;
}

KM_INLINE Void Vec2::Zero( Void ) {
  x = y = 0.0f;
}

KM_INLINE Void Vec2::Rotate90CCW() {
  this->Set( this->y, -this->x );
}

KM_INLINE Void Vec2::Rotate90CW() {
  this->Set( -this->y, this->x );
}

KM_INLINE Vec2 Vec2::Projection( Vec2 &a ) {
  return a * ( a.Dot( *this ) / a.LengthSqr() );
}

KM_INLINE Float Vec2::Dot( const Vec2 &a ) const {
  return this->x * a.x + this->y * a.y;
}

KM_INLINE Bool Vec2::Compare( const Vec2 &a ) const {
  return ( ( x == a.x ) && ( y == a.y ) );
}

KM_INLINE Bool Vec2::Compare( const Vec2 &a, const Float epsilon ) const {
  if ( Math::Fabs( x - a.x ) > epsilon ) {
    return false;
  }
      
  if ( Math::Fabs( y - a.y ) > epsilon ) {
    return false;
  }

  return true;
}

KM_INLINE Bool Vec2::operator==( const Vec2 &a ) const {
  return Compare( a );
}

KM_INLINE Bool Vec2::operator!=( const Vec2 &a ) const {
  return !Compare( a );
}

KM_INLINE Float Vec2::operator[]( Int index ) const {
  return ( &x )[ index ];
}

KM_INLINE Float& Vec2::operator[]( Int index ) {
  return ( &x )[ index ];
}

KM_INLINE bool Vec2::IsLess( const Vec2 &a ) const {
  return x < a.x - Math::FLT_EPSILON_NUM || Math::Fabs( x - a.x ) < Math::FLT_EPSILON_NUM && y < a.y - Math::FLT_EPSILON_NUM;
}

KM_INLINE float Vec2::IntersectDet( float a, float b, float c, float d ) {
  return  ( a * d - b * c );
}

KM_INLINE Float Vec2::Length( Void ) const {
  return ( Float )Math::Sqrt( x * x + y * y );
}

KM_INLINE Float Vec2::LengthFast( Void ) const {
  Float sqrLength;

  sqrLength = x * x + y * y;
  return sqrLength * Math::RSqrt( sqrLength );
}

KM_INLINE Float Vec2::LengthSqr( Void ) const {
  return ( x * x + y * y );
}

KM_INLINE Float Vec2::Normalize( Void ) {
  Float sqrLength, invLength;

  sqrLength = x * x + y * y;
  invLength = Math::InvSqrt( sqrLength );
  x *= invLength;
  y *= invLength;
  return invLength * sqrLength;
}

KM_INLINE Float Vec2::NormalizeFast( Void ) {
  Float lengthSqr, invLength;

  lengthSqr = x * x + y * y;
  invLength = Math::RSqrt( lengthSqr );
  x *= invLength;
  y *= invLength;
  return invLength * lengthSqr;
}

KM_INLINE Vec2 &Vec2::Truncate( Float length ) {
  Float length2;
  Float ilength;

  if ( !length ) {
    Zero();
  }
  else {
    length2 = LengthSqr();
    if ( length2 > length * length ) {
      ilength = length * Math::InvSqrt( length2 );
      x *= ilength;
      y *= ilength;
    }
  }

  return *this;
}

KM_INLINE Void Vec2::Clamp( const Vec2 &min, const Vec2 &max ) {
  if ( x < min.x ) {
    x = min.x;
  } else if ( x > max.x ) {
    x = max.x;
  }
  if ( y < min.y ) {
    y = min.y;
  } else if ( y > max.y ) {
    y = max.y;
  }
}

KM_INLINE Void Vec2::Snap( Void ) {
  x = floor( x + 0.5f );
  y = floor( y + 0.5f );
}

KM_INLINE Void Vec2::SnapInt( Void ) {
  x = Float( Int( x ) );
  y = Float( Int( y ) );
}

KM_INLINE Vec2 Vec2::operator-() const {
  return Vec2( -x, -y );
}
  
KM_INLINE Vec2 Vec2::operator-( const Vec2 &a ) const {
  return Vec2( x - a.x, y - a.y );
}

KM_INLINE Float Vec2::operator*( const Vec2 &a ) const {
  return x * a.x + y * a.y;
}

KM_INLINE Vec2 Vec2::operator*( const Float a ) const {
  return Vec2( x * a, y * a );
}

KM_INLINE Vec2 Vec2::operator/( const Float a ) const {
  Float inva = 1.0f / a;
  return Vec2( x * inva, y * inva );
}

KM_INLINE Vec2 operator*( const Float a, const Vec2 b ) {
  return Vec2( b.x * a, b.y * a );
}

KM_INLINE Vec2 Vec2::operator+( const Vec2 &a ) const {
  return Vec2( x + a.x, y + a.y );
}

KM_INLINE Vec2 &Vec2::operator+=( const Vec2 &a ) {
  x += a.x;
  y += a.y;

  return *this;
}

KM_INLINE Vec2 &Vec2::operator/=( const Vec2 &a ) {
  x /= a.x;
  y /= a.y;

  return *this;
}

KM_INLINE Vec2 &Vec2::operator/=( const Float a ) {
  Float inva = 1.0f / a;
  x *= inva;
  y *= inva;

  return *this;
}

KM_INLINE Vec2 &Vec2::operator-=( const Vec2 &a ) {
  x -= a.x;
  y -= a.y;

  return *this;
}

KM_INLINE Vec2 &Vec2::operator*=( const Float a ) {
  x *= a;
  y *= a;

  return *this;
}

KM_INLINE Int Vec2::GetDimension( Void ) const {
  return 2;
}

KM_INLINE const Float *Vec2::ToFloatPtr( Void ) const {
  return &x;
}

KM_INLINE Float *Vec2::ToFloatPtr( Void ) {
  return &x;
}


//===============================================================
//
//  Vec3 - 3D vector
//
//===============================================================

class Vec3 {
public:
  Float     x;
  Float     y;
  Float     z;

  Vec3( Void );
  explicit Vec3( const Float x, const Float y, const Float z );

  Void      Set( const Float x, const Float y, const Float z );
  Void      Zero( Void );

  Float     operator[]( const Int index ) const;
  Float &   operator[]( const Int index );
  Vec3      operator-() const;
  Vec3 &    operator=( const Vec3 &a );    // required because of a msvc 6 & 7 bug
  Float     operator*( const Vec3 &a ) const;
  Vec3      operator*( const Float a ) const;
  Vec3      operator/( const Float a ) const;
  Vec3      operator+( const Vec3 &a ) const;
  Vec3      operator-( const Vec3 &a ) const;
  Vec3 &    operator+=( const Vec3 &a );
  Vec3 &    operator-=( const Vec3 &a );
  Vec3 &    operator/=( const Vec3 &a );
  Vec3 &    operator/=( const Float a );
  Vec3 &    operator*=( const Float a );

  friend Vec3  operator*( const Float a, const Vec3 b );

  Bool      Compare( const Vec3 &a ) const;              // exact compare, no epsilon
  Bool      Compare( const Vec3 &a, const Float epsilon ) const;    // compare with epsilon
  Bool      operator==(  const Vec3 &a ) const;            // exact compare, no epsilon
  Bool      operator!=(  const Vec3 &a ) const;            // exact compare, no epsilon

  Bool      FixDegenerateNormal( Void );  // fix degenerate axial cases
  Bool      FixDenormals( Void );      // change tiny numbers to zero

  Vec3      Cross( const Vec3 &a ) const;
  Vec3 &    Cross( const Vec3 &a, const Vec3 &b );
  Float     Length( Void ) const;
  Float     LengthSqr( Void ) const;
  Float     LengthFast( Void ) const;
  Float     Normalize( Void );        // returns length
  Float     NormalizeFast( Void );      // returns length
  Vec3 &    Truncate( Float length );    // cap length
  Void      Clamp( const Vec3 &min, const Vec3 &max );
  Void      Snap( Void );          // snap to closest integer value
  Void      SnapInt( Void );        // snap towards integer (floor)

  Int       GetDimension( Void ) const;

  Float     ToYaw( Void ) const;
  Float     ToPitch( Void ) const;
  Angles    ToAngles( Void ) const;
  Polar3    ToPolar( Void ) const;
  Mat3      ToMat3( Void ) const;    // vector should be normalized
  const Vec2 &  ToVec2( Void ) const;
  Vec2 &    ToVec2( Void );
  const Float *  ToFloatPtr( Void ) const;
  Float *   ToFloatPtr( Void );

  Void      NormalVectors( Vec3 &left, Vec3 &down ) const;  // vector should be normalized
  Void      OrthogonalBasis( Vec3 &left, Vec3 &up ) const;

  Void      ProjectOntoPlane( const Vec3 &normal, const Float overBounce = 1.0f );
  Bool      ProjectAlongPlane( const Vec3 &normal, const Float epsilon, const Float overBounce = 1.0f );
  Void      ProjectSelfOntoSphere( const Float radius );

  Void      Lerp( const Vec3 &v1, const Vec3 &v2, const Float l );
  Void      SLerp( const Vec3 &v1, const Vec3 &v2, const Float l );
};

KM_INLINE Vec3::Vec3( Void ) {
}

KM_INLINE Vec3::Vec3( const Float x, const Float y, const Float z ) {
  this->x = x;
  this->y = y;
  this->z = z;
}

KM_INLINE Float Vec3::operator[]( const Int index ) const {
  return ( &x )[ index ];
}

KM_INLINE Float &Vec3::operator[]( const Int index ) {
  return ( &x )[ index ];
}

KM_INLINE Void Vec3::Set( const Float x, const Float y, const Float z ) {
  this->x = x;
  this->y = y;
  this->z = z;
}

KM_INLINE Void Vec3::Zero( Void ) {
  x = y = z = 0.0f;
}

KM_INLINE Vec3 Vec3::operator-() const {
  return Vec3( -x, -y, -z );
}

KM_INLINE Vec3 &Vec3::operator=( const Vec3 &a ) {
  x = a.x;
  y = a.y;
  z = a.z;
  return *this;
}

KM_INLINE Vec3 Vec3::operator-( const Vec3 &a ) const {
  return Vec3( x - a.x, y - a.y, z - a.z );
}

KM_INLINE Float Vec3::operator*( const Vec3 &a ) const {
  return x * a.x + y * a.y + z * a.z;
}

KM_INLINE Vec3 Vec3::operator*( const Float a ) const {
  return Vec3( x * a, y * a, z * a );
}

KM_INLINE Vec3 Vec3::operator/( const Float a ) const {
  Float inva = 1.0f / a;
  return Vec3( x * inva, y * inva, z * inva );
}

KM_INLINE Vec3 operator*( const Float a, const Vec3 b ) {
  return Vec3( b.x * a, b.y * a, b.z * a );
}

KM_INLINE Vec3 Vec3::operator+( const Vec3 &a ) const {
  return Vec3( x + a.x, y + a.y, z + a.z );
}

KM_INLINE Vec3 &Vec3::operator+=( const Vec3 &a ) {
  x += a.x;
  y += a.y;
  z += a.z;

  return *this;
}

KM_INLINE Vec3 &Vec3::operator/=( const Vec3 &a ) {
  x /= a.x;
  y /= a.y;
  z /= a.z;

  return *this;
}

KM_INLINE Vec3 &Vec3::operator/=( const Float a ) {
  Float inva = 1.0f / a;
  x *= inva;
  y *= inva;
  z *= inva;

  return *this;
}

KM_INLINE Vec3 &Vec3::operator-=( const Vec3 &a ) {
  x -= a.x;
  y -= a.y;
  z -= a.z;

  return *this;
}

KM_INLINE Vec3 &Vec3::operator*=( const Float a ) {
  x *= a;
  y *= a;
  z *= a;

  return *this;
}

KM_INLINE Bool Vec3::Compare( const Vec3 &a ) const {
  return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

KM_INLINE Bool Vec3::Compare( const Vec3 &a, const Float epsilon ) const {
  if ( Math::Fabs( x - a.x ) > epsilon ) {
    return false;
  }
      
  if ( Math::Fabs( y - a.y ) > epsilon ) {
    return false;
  }

  if ( Math::Fabs( z - a.z ) > epsilon ) {
    return false;
  }

  return true;
}

KM_INLINE Bool Vec3::operator==( const Vec3 &a ) const {
  return Compare( a );
}

KM_INLINE Bool Vec3::operator!=( const Vec3 &a ) const {
  return !Compare( a );
}

KM_INLINE Float Vec3::NormalizeFast( Void ) {
  Float sqrLength, invLength;

  sqrLength = x * x + y * y + z * z;
  invLength = Math::RSqrt( sqrLength );
  x *= invLength;
  y *= invLength;
  z *= invLength;
  return invLength * sqrLength;
}

KM_INLINE Bool Vec3::FixDegenerateNormal( Void ) {
  if ( x == 0.0f ) {
    if ( y == 0.0f ) {
      if ( z > 0.0f ) {
        if ( z != 1.0f ) {
          z = 1.0f;
          return true;
        }
      } else {
        if ( z != -1.0f ) {
          z = -1.0f;
          return true;
        }
      }
      return false;
    } else if ( z == 0.0f ) {
      if ( y > 0.0f ) {
        if ( y != 1.0f ) {
          y = 1.0f;
          return true;
        }
      } else {
        if ( y != -1.0f ) {
          y = -1.0f;
          return true;
        }
      }
      return false;
    }
  } else if ( y == 0.0f ) {
    if ( z == 0.0f ) {
      if ( x > 0.0f ) {
        if ( x != 1.0f ) {
          x = 1.0f;
          return true;
        }
      } else {
        if ( x != -1.0f ) {
          x = -1.0f;
          return true;
        }
      }
      return false;
    }
  }
  if ( Math::Fabs( x ) == 1.0f ) {
    if ( y != 0.0f || z != 0.0f ) {
      y = z = 0.0f;
      return true;
    }
    return false;
  } else if ( Math::Fabs( y ) == 1.0f ) {
    if ( x != 0.0f || z != 0.0f ) {
      x = z = 0.0f;
      return true;
    }
    return false;
  } else if ( Math::Fabs( z ) == 1.0f ) {
    if ( x != 0.0f || y != 0.0f ) {
      x = y = 0.0f;
      return true;
    }
    return false;
  }
  return false;
}

KM_INLINE Bool Vec3::FixDenormals( Void ) {
  Bool denormal = false;
  if ( fabs( x ) < 1e-30f ) {
    x = 0.0f;
    denormal = true;
  }
  if ( fabs( y ) < 1e-30f ) {
    y = 0.0f;
    denormal = true;
  }
  if ( fabs( z ) < 1e-30f ) {
    z = 0.0f;
    denormal = true;
  }
  return denormal;
}

KM_INLINE Vec3 Vec3::Cross( const Vec3 &a ) const {
  return Vec3( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x );
}

KM_INLINE Vec3 &Vec3::Cross( const Vec3 &a, const Vec3 &b ) {
  x = a.y * b.z - a.z * b.y;
  y = a.z * b.x - a.x * b.z;
  z = a.x * b.y - a.y * b.x;

  return *this;
}

KM_INLINE Float Vec3::Length( Void ) const {
  return ( Float )Math::Sqrt( x * x + y * y + z * z );
}

KM_INLINE Float Vec3::LengthSqr( Void ) const {
  return ( x * x + y * y + z * z );
}

KM_INLINE Float Vec3::LengthFast( Void ) const {
  Float sqrLength;

  sqrLength = x * x + y * y + z * z;
  return sqrLength * Math::RSqrt( sqrLength );
}

KM_INLINE Float Vec3::Normalize( Void ) {
  Float sqrLength, invLength;

  sqrLength = x * x + y * y + z * z;
  invLength = Math::InvSqrt( sqrLength );
  x *= invLength;
  y *= invLength;
  z *= invLength;
  return invLength * sqrLength;
}

KM_INLINE Vec3 &Vec3::Truncate( Float length ) {
  Float length2;
  Float ilength;

  if ( !length ) {
    Zero();
  }
  else {
    length2 = LengthSqr();
    if ( length2 > length * length ) {
      ilength = length * Math::InvSqrt( length2 );
      x *= ilength;
      y *= ilength;
      z *= ilength;
    }
  }

  return *this;
}

KM_INLINE Void Vec3::Clamp( const Vec3 &min, const Vec3 &max ) {
  if ( x < min.x ) {
    x = min.x;
  } else if ( x > max.x ) {
    x = max.x;
  }
  if ( y < min.y ) {
    y = min.y;
  } else if ( y > max.y ) {
    y = max.y;
  }
  if ( z < min.z ) {
    z = min.z;
  } else if ( z > max.z ) {
    z = max.z;
  }
}

KM_INLINE Void Vec3::Snap( Void ) {
  x = floor( x + 0.5f );
  y = floor( y + 0.5f );
  z = floor( z + 0.5f );
}

KM_INLINE Void Vec3::SnapInt( Void ) {
  x = Float( Int( x ) );
  y = Float( Int( y ) );
  z = Float( Int( z ) );
}

KM_INLINE Int Vec3::GetDimension( Void ) const {
  return 3;
}

KM_INLINE const Vec2 &Vec3::ToVec2( Void ) const {
  return *reinterpret_cast<const Vec2 *>(this);
}

KM_INLINE Vec2 &Vec3::ToVec2( Void ) {
  return *reinterpret_cast<Vec2 *>(this);
}

KM_INLINE const Float *Vec3::ToFloatPtr( Void ) const {
  return &x;
}

KM_INLINE Float *Vec3::ToFloatPtr( Void ) {
  return &x;
}

KM_INLINE Void Vec3::NormalVectors( Vec3 &left, Vec3 &down ) const {
  Float d;

  d = x * x + y * y;
  if ( !d ) {
    left[0] = 1;
    left[1] = 0;
    left[2] = 0;
  } else {
    d = Math::InvSqrt( d );
    left[0] = -y * d;
    left[1] = x * d;
    left[2] = 0;
  }
  down = left.Cross( *this );
}

KM_INLINE Void Vec3::OrthogonalBasis( Vec3 &left, Vec3 &up ) const {
  Float l, s;

  if ( Math::Fabs( z ) > 0.7f ) {
    l = y * y + z * z;
    s = Math::InvSqrt( l );
    up[0] = 0;
    up[1] = z * s;
    up[2] = -y * s;
    left[0] = l * s;
    left[1] = -x * up[2];
    left[2] = x * up[1];
  }
  else {
    l = x * x + y * y;
    s = Math::InvSqrt( l );
    left[0] = -y * s;
    left[1] = x * s;
    left[2] = 0;
    up[0] = -z * left[1];
    up[1] = z * left[0];
    up[2] = l * s;
  }
}

KM_INLINE Void Vec3::ProjectOntoPlane( const Vec3 &normal, const Float overBounce ) {
  Float backoff;
  
  backoff = *this * normal;
  
  if ( overBounce != 1.0 ) {
    if ( backoff < 0 ) {
      backoff *= overBounce;
    } else {
      backoff /= overBounce;
    }
  }

  *this -= backoff * normal;
}

KM_INLINE Bool Vec3::ProjectAlongPlane( const Vec3 &normal, const Float epsilon, const Float overBounce ) {
  Vec3 cross;
  Float len;

  cross = this->Cross( normal ).Cross( (*this) );
  // normalize so a fixed epsilon can be used
  cross.Normalize();
  len = normal * cross;
  if ( Math::Fabs( len ) < epsilon ) {
    return false;
  }
  cross *= overBounce * ( normal * (*this) ) / len;
  (*this) -= cross;
  return true;
}


//===============================================================
//
//  Vec4 - 4D vector
//
//===============================================================

class Vec4 {
public:
  Float     x;
  Float     y;
  Float     z;
  Float     w;

  Vec4( Void );
  explicit Vec4( const Float x, const Float y, const Float z, const Float w );

  Void      Set( const Float x, const Float y, const Float z, const Float w );
  Void      Zero( Void );

  Float     operator[]( const Int index ) const;
  Float &   operator[]( const Int index );
  Vec4      operator-() const;
  Float     operator*( const Vec4 &a ) const;
  Vec4      operator*( const Float a ) const;
  Vec4      operator/( const Float a ) const;
  Vec4      operator+( const Vec4 &a ) const;
  Vec4      operator-( const Vec4 &a ) const;
  Vec4 &    operator+=( const Vec4 &a );
  Vec4 &    operator-=( const Vec4 &a );
  Vec4 &    operator/=( const Vec4 &a );
  Vec4 &    operator/=( const Float a );
  Vec4 &    operator*=( const Float a );

  friend Vec4  operator*( const Float a, const Vec4 b );

  Bool      Compare( const Vec4 &a ) const;              // exact compare, no epsilon
  Bool      Compare( const Vec4 &a, const Float epsilon ) const;    // compare with epsilon
  Bool      operator==(  const Vec4 &a ) const;            // exact compare, no epsilon
  Bool      operator!=(  const Vec4 &a ) const;            // exact compare, no epsilon

  Float     Length( Void ) const;
  Float     LengthSqr( Void ) const;
  Float     Normalize( Void );      // returns length
  Float     NormalizeFast( Void );    // returns length

  Int       GetDimension( Void ) const;

  const Vec2 &  ToVec2( Void ) const;
  Vec2 &    ToVec2( Void );
  const Vec3 &  ToVec3( Void ) const;
  Vec3 &    ToVec3( Void );
  const Float *  ToFloatPtr( Void ) const;
  Float *   ToFloatPtr( Void );

  Void      Lerp( const Vec4 &v1, const Vec4 &v2, const Float l );
};

KM_INLINE Vec4::Vec4( Void ) {
}

KM_INLINE Vec4::Vec4( const Float x, const Float y, const Float z, const Float w ) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

KM_INLINE Void Vec4::Set( const Float x, const Float y, const Float z, const Float w ) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

KM_INLINE Void Vec4::Zero( Void ) {
  x = y = z = w = 0.0f;
}

KM_INLINE Float Vec4::operator[]( Int index ) const {
  return ( &x )[ index ];
}

KM_INLINE Float& Vec4::operator[]( Int index ) {
  return ( &x )[ index ];
}

KM_INLINE Vec4 Vec4::operator-() const {
  return Vec4( -x, -y, -z, -w );
}

KM_INLINE Vec4 Vec4::operator-( const Vec4 &a ) const {
  return Vec4( x - a.x, y - a.y, z - a.z, w - a.w );
}

KM_INLINE Float Vec4::operator*( const Vec4 &a ) const {
  return x * a.x + y * a.y + z * a.z + w * a.w;
}

KM_INLINE Vec4 Vec4::operator*( const Float a ) const {
  return Vec4( x * a, y * a, z * a, w * a );
}

KM_INLINE Vec4 Vec4::operator/( const Float a ) const {
  Float inva = 1.0f / a;
  return Vec4( x * inva, y * inva, z * inva, w * inva );
}

KM_INLINE Vec4 operator*( const Float a, const Vec4 b ) {
  return Vec4( b.x * a, b.y * a, b.z * a, b.w * a );
}

KM_INLINE Vec4 Vec4::operator+( const Vec4 &a ) const {
  return Vec4( x + a.x, y + a.y, z + a.z, w + a.w );
}

KM_INLINE Vec4 &Vec4::operator+=( const Vec4 &a ) {
  x += a.x;
  y += a.y;
  z += a.z;
  w += a.w;

  return *this;
}

KM_INLINE Vec4 &Vec4::operator/=( const Vec4 &a ) {
  x /= a.x;
  y /= a.y;
  z /= a.z;
  w /= a.w;

  return *this;
}

KM_INLINE Vec4 &Vec4::operator/=( const Float a ) {
  Float inva = 1.0f / a;
  x *= inva;
  y *= inva;
  z *= inva;
  w *= inva;

  return *this;
}

KM_INLINE Vec4 &Vec4::operator-=( const Vec4 &a ) {
  x -= a.x;
  y -= a.y;
  z -= a.z;
  w -= a.w;

  return *this;
}

KM_INLINE Vec4 &Vec4::operator*=( const Float a ) {
  x *= a;
  y *= a;
  z *= a;
  w *= a;

  return *this;
}

KM_INLINE Bool Vec4::Compare( const Vec4 &a ) const {
  return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && w == a.w );
}

KM_INLINE Bool Vec4::Compare( const Vec4 &a, const Float epsilon ) const {
  if ( Math::Fabs( x - a.x ) > epsilon ) {
    return false;
  }
      
  if ( Math::Fabs( y - a.y ) > epsilon ) {
    return false;
  }

  if ( Math::Fabs( z - a.z ) > epsilon ) {
    return false;
  }

  if ( Math::Fabs( w - a.w ) > epsilon ) {
    return false;
  }

  return true;
}

KM_INLINE Bool Vec4::operator==( const Vec4 &a ) const {
  return Compare( a );
}

KM_INLINE Bool Vec4::operator!=( const Vec4 &a ) const {
  return !Compare( a );
}

KM_INLINE Float Vec4::Length( Void ) const {
  return ( Float )Math::Sqrt( x * x + y * y + z * z + w * w );
}

KM_INLINE Float Vec4::LengthSqr( Void ) const {
  return ( x * x + y * y + z * z + w * w );
}

KM_INLINE Float Vec4::Normalize( Void ) {
  Float sqrLength, invLength;

  sqrLength = x * x + y * y + z * z + w * w;
  invLength = Math::InvSqrt( sqrLength );
  x *= invLength;
  y *= invLength;
  z *= invLength;
  w *= invLength;
  return invLength * sqrLength;
}

KM_INLINE Float Vec4::NormalizeFast( Void ) {
  Float sqrLength, invLength;

  sqrLength = x * x + y * y + z * z + w * w;
  invLength = Math::RSqrt( sqrLength );
  x *= invLength;
  y *= invLength;
  z *= invLength;
  w *= invLength;
  return invLength * sqrLength;
}

KM_INLINE Int Vec4::GetDimension( Void ) const {
  return 4;
}

KM_INLINE const Vec2 &Vec4::ToVec2( Void ) const {
  return *reinterpret_cast<const Vec2 *>(this);
}

KM_INLINE Vec2 &Vec4::ToVec2( Void ) {
  return *reinterpret_cast<Vec2 *>(this);
}

KM_INLINE const Vec3 &Vec4::ToVec3( Void ) const {
  return *reinterpret_cast<const Vec3 *>(this);
}

KM_INLINE Vec3 &Vec4::ToVec3( Void ) {
  return *reinterpret_cast<Vec3 *>(this);
}

KM_INLINE const Float *Vec4::ToFloatPtr( Void ) const {
  return &x;
}

KM_INLINE Float *Vec4::ToFloatPtr( Void ) {
  return &x;
}




class Polar3 {
public:
  Float     radius, theta, phi;

  Polar3( Void );
  explicit Polar3( const Float radius, const Float theta, const Float phi );

  Void      Set( const Float radius, const Float theta, const Float phi );

  Float     operator[]( const int index ) const;
  Float &   operator[]( const int index );
  Polar3    operator-() const;
  Polar3 &  operator=( const Polar3 &a );

  Vec3      ToVec3( Void ) const;
};

KM_INLINE Polar3::Polar3( Void ) {
}

KM_INLINE Polar3::Polar3( const Float radius, const Float theta, const Float phi ) {
  assert( radius > 0 );
  this->radius = radius;
  this->theta = theta;
  this->phi = phi;
}
  
KM_INLINE Void Polar3::Set( const Float radius, const Float theta, const Float phi ) {
  assert( radius > 0 );
  this->radius = radius;
  this->theta = theta;
  this->phi = phi;
}

KM_INLINE Float Polar3::operator[]( const int index ) const {
  return ( &radius )[ index ];
}

KM_INLINE Float &Polar3::operator[]( const int index ) {
  return ( &radius )[ index ];
}

KM_INLINE Polar3 Polar3::operator-() const {
  return Polar3( radius, -theta, -phi );
}

KM_INLINE Polar3 &Polar3::operator=( const Polar3 &a ) { 
  radius = a.radius;
  theta = a.theta;
  phi = a.phi;
  return *this;
}

KM_INLINE Vec3 Polar3::ToVec3( Void ) const {
  Float sp, cp, st, ct;
  Math::SinCos( phi, sp, cp );
  Math::SinCos( theta, st, ct );
   return Vec3( cp * radius * ct, cp * radius * st, radius * sp );
}




class Line2 {
public:
	float a, b, c;
 
	Line2();
	Line2 (Vec2 p, Vec2 q);
	void Normalize();
	float Dist( Vec2 p ) const;
};


extern Vec2 Vec2Null;
extern Vec2 Vec2One;
extern Vec3 Vec3Null;
extern Vec3 Vec3One;
extern Vec4 Vec4Null;
extern Vec4 Vec4One;
