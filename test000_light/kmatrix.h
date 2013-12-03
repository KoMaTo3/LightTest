#pragma once

#include "types.h"
#include "kvector.h"
#include "klib.h"

class Vec2;
class Mat4;

#define MATRIX_INVERSE_EPSILON  1e-14
#define MATRIX_EPSILON          1e-6

class Mat2 {
public:
  Mat2( Void );
  explicit Mat2( const Vec2 &x, const Vec2 &y );
  explicit Mat2( const Float xx, const Float xy, const Float yx, const Float yy );
  explicit Mat2( const Float src[ 2 ][ 2 ] );

  const Vec2 &  operator[]( Int index ) const;
  Vec2 &    operator[]( Int index );
  Mat2      operator-() const;
  Mat2      operator*( const Float a ) const;
  Vec2      operator*( const Vec2 &vec ) const;
  Mat2      operator*( const Mat2 &a ) const;
  Mat2      operator+( const Mat2 &a ) const;
  Mat2      operator-( const Mat2 &a ) const;
  Mat2 &    operator*=( const Float a );
  Mat2 &    operator*=( const Mat2 &a );
  Mat2 &    operator+=( const Mat2 &a );
  Mat2 &    operator-=( const Mat2 &a );

  friend Mat2  operator*( const Float a, const Mat2 &mat );
  friend Vec2  operator*( const Vec2 &vec, const Mat2 &mat );
  friend Vec2 &  operator*=( Vec2 &vec, const Mat2 &mat );

  Bool      Compare( const Mat2 &a ) const;            // exact compare, no epsilon
  Bool      Compare( const Mat2 &a, const Float epsilon ) const;  // compare with epsilon
  Bool      operator==( const Mat2 &a ) const;          // exact compare, no epsilon
  Bool      operator!=( const Mat2 &a ) const;          // exact compare, no epsilon

  Void      Zero( Void );
  Void      Identity( Void );
  Bool      IsIdentity( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsSymmetric( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsDiagonal( const Float epsilon = MATRIX_EPSILON ) const;

  Float     Trace( Void ) const;
  Float     Determinant( Void ) const;
  Mat2      Transpose( Void ) const;  // returns transpose
  Mat2 &    TransposeSelf( Void );
  Mat2      Inverse( Void ) const;    // returns the inverse ( m * m.Inverse() = identity )
  Bool      InverseSelf( Void );    // returns false if determinant is zero
  Mat2      InverseFast( Void ) const;  // returns the inverse ( m * m.Inverse() = identity )
  Bool      InverseFastSelf( Void );  // returns false if determinant is zero

  Int       GetDimension( Void ) const;

  const Float *  ToFloatPtr( Void ) const;
  Float *   ToFloatPtr( Void );

private:
  Vec2      mat[ 2 ];
};

extern Mat2 mat2_zero;
extern Mat2 mat2_identity;
#define mat2_default  mat2_identity

KM_INLINE Mat2::Mat2( Void ) {
}

KM_INLINE Mat2::Mat2( const Vec2 &x, const Vec2 &y ) {
  mat[ 0 ].x = x.x; mat[ 0 ].y = x.y;
  mat[ 1 ].x = y.x; mat[ 1 ].y = y.y;
}

KM_INLINE Mat2::Mat2( const Float xx, const Float xy, const Float yx, const Float yy ) {
  mat[ 0 ].x = xx; mat[ 0 ].y = xy;
  mat[ 1 ].x = yx; mat[ 1 ].y = yy;
}

KM_INLINE Mat2::Mat2( const Float src[ 2 ][ 2 ] ) {
  memcpy( mat, src, 2 * 2 * sizeof( Float ) );
}

KM_INLINE const Vec2 &Mat2::operator[]( Int index ) const {
  assert( ( index >= 0 ) && ( index < 2 ) );
  return mat[ index ];
}

KM_INLINE Vec2 &Mat2::operator[]( Int index ) {
  assert( ( index >= 0 ) && ( index < 2 ) );
  return mat[ index ];
}

KM_INLINE Mat2 Mat2::operator-() const {
  return Mat2(  -mat[0][0], -mat[0][1],
          -mat[1][0], -mat[1][1] );
}

KM_INLINE Vec2 Mat2::operator*( const Vec2 &vec ) const {
  return Vec2(
    mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y,
    mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y );
}

KM_INLINE Mat2 Mat2::operator*( const Mat2 &a ) const {
  return Mat2(
    mat[0].x * a[0].x + mat[0].y * a[1].x,
    mat[0].x * a[0].y + mat[0].y * a[1].y,
    mat[1].x * a[0].x + mat[1].y * a[1].x,
    mat[1].x * a[0].y + mat[1].y * a[1].y );
}

KM_INLINE Mat2 Mat2::operator*( const Float a ) const {
  return Mat2(
    mat[0].x * a, mat[0].y * a, 
    mat[1].x * a, mat[1].y * a );
}

KM_INLINE Mat2 Mat2::operator+( const Mat2 &a ) const {
  return Mat2(
    mat[0].x + a[0].x, mat[0].y + a[0].y, 
    mat[1].x + a[1].x, mat[1].y + a[1].y );
}
    
KM_INLINE Mat2 Mat2::operator-( const Mat2 &a ) const {
  return Mat2(
    mat[0].x - a[0].x, mat[0].y - a[0].y,
    mat[1].x - a[1].x, mat[1].y - a[1].y );
}

KM_INLINE Mat2 &Mat2::operator*=( const Float a ) {
  mat[0].x *= a; mat[0].y *= a;
  mat[1].x *= a; mat[1].y *= a;

    return *this;
}

KM_INLINE Mat2 &Mat2::operator*=( const Mat2 &a ) {
  Float x, y;
  x = mat[0].x; y = mat[0].y;
  mat[0].x = x * a[0].x + y * a[1].x;
  mat[0].y = x * a[0].y + y * a[1].y;
  x = mat[1].x; y = mat[1].y;
  mat[1].x = x * a[0].x + y * a[1].x;
  mat[1].y = x * a[0].y + y * a[1].y;
  return *this;
}

KM_INLINE Mat2 &Mat2::operator+=( const Mat2 &a ) {
  mat[0].x += a[0].x; mat[0].y += a[0].y;
  mat[1].x += a[1].x; mat[1].y += a[1].y;

    return *this;
}

KM_INLINE Mat2 &Mat2::operator-=( const Mat2 &a ) {
  mat[0].x -= a[0].x; mat[0].y -= a[0].y;
  mat[1].x -= a[1].x; mat[1].y -= a[1].y;

    return *this;
}

KM_INLINE Vec2 operator*( const Vec2 &vec, const Mat2 &mat ) {
  return mat * vec;
}

KM_INLINE Mat2 operator*( const Float a, Mat2 const &mat ) {
  return mat * a;
}

KM_INLINE Vec2 &operator*=( Vec2 &vec, const Mat2 &mat ) {
  vec = mat * vec;
  return vec;
}

KM_INLINE Bool Mat2::Compare( const Mat2 &a ) const {
  if ( mat[0].Compare( a[0] ) &&
    mat[1].Compare( a[1] ) ) {
    return true;
  }
  return false;
}

KM_INLINE Bool Mat2::Compare( const Mat2 &a, const Float epsilon ) const {
  if ( mat[0].Compare( a[0], epsilon ) &&
    mat[1].Compare( a[1], epsilon ) ) {
    return true;
  }
  return false;
}

KM_INLINE Bool Mat2::operator==( const Mat2 &a ) const {
  return Compare( a );
}

KM_INLINE Bool Mat2::operator!=( const Mat2 &a ) const {
  return !Compare( a );
}

KM_INLINE Void Mat2::Zero( Void ) {
  mat[0].Zero();
  mat[1].Zero();
}

KM_INLINE Void Mat2::Identity( Void ) {
  *this = mat2_identity;
}

KM_INLINE Bool Mat2::IsIdentity( const Float epsilon ) const {
  return Compare( mat2_identity, epsilon );
}

KM_INLINE Bool Mat2::IsSymmetric( const Float epsilon ) const {
  return ( Math::Fabs( mat[0][1] - mat[1][0] ) < epsilon );
}

KM_INLINE Bool Mat2::IsDiagonal( const Float epsilon ) const {
  if ( Math::Fabs( mat[0][1] ) > epsilon ||
    Math::Fabs( mat[1][0] ) > epsilon ) {
    return false;
  }
  return true;
}

KM_INLINE Float Mat2::Trace( Void ) const {
  return ( mat[0][0] + mat[1][1] );
}

KM_INLINE Float Mat2::Determinant( Void ) const {
  return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

KM_INLINE Mat2 Mat2::Transpose( Void ) const {
  return Mat2(  mat[0][0], mat[1][0],
          mat[0][1], mat[1][1] );
}

KM_INLINE Mat2 &Mat2::TransposeSelf( Void ) {
  Float tmp;

  tmp = mat[0][1];
  mat[0][1] = mat[1][0];
  mat[1][0] = tmp;

  return *this;
}

KM_INLINE Mat2 Mat2::Inverse( Void ) const {
  Mat2 invMat;

  invMat = *this;
  Int r = invMat.InverseSelf();
  assert( r );
  return invMat;
}

KM_INLINE Mat2 Mat2::InverseFast( Void ) const {
  Mat2 invMat;

  invMat = *this;
  Int r = invMat.InverseFastSelf();
  assert( r );
  return invMat;
}

KM_INLINE Int Mat2::GetDimension( Void ) const {
  return 4;
}

KM_INLINE const Float *Mat2::ToFloatPtr( Void ) const {
  return mat[0].ToFloatPtr();
}

KM_INLINE Float *Mat2::ToFloatPtr( Void ) {
  return mat[0].ToFloatPtr();
}


//===============================================================
//
//  Mat3 - 3x3 matrix
//
//  NOTE:  matrix is column-major
//
//===============================================================

class Mat3 {
public:
  Mat3( Void );
  explicit Mat3( const Vec3 &x, const Vec3 &y, const Vec3 &z );
  explicit Mat3( const Float xx, const Float xy, const Float xz, const Float yx, const Float yy, const Float yz, const Float zx, const Float zy, const Float zz );
  explicit Mat3( const Float src[ 3 ][ 3 ] );

  const Vec3 &  operator[]( Int index ) const;
  Vec3 &    operator[]( Int index );
  Mat3      operator-() const;
  Mat3      operator*( const Float a ) const;
  Vec3      operator*( const Vec3 &vec ) const;
  Mat3      operator*( const Mat3 &a ) const;
  Mat3      operator+( const Mat3 &a ) const;
  Mat3      operator-( const Mat3 &a ) const;
  Mat3 &    operator*=( const Float a );
  Mat3 &    operator*=( const Mat3 &a );
  Mat3 &    operator+=( const Mat3 &a );
  Mat3 &    operator-=( const Mat3 &a );

  friend Mat3  operator*( const Float a, const Mat3 &mat );
  friend Vec3  operator*( const Vec3 &vec, const Mat3 &mat );
  friend Vec3 &operator*=( Vec3 &vec, const Mat3 &mat );

  Bool      Compare( const Mat3 &a ) const;            // exact compare, no epsilon
  Bool      Compare( const Mat3 &a, const Float epsilon ) const;  // compare with epsilon
  Bool      operator==( const Mat3 &a ) const;          // exact compare, no epsilon
  Bool      operator!=( const Mat3 &a ) const;          // exact compare, no epsilon

  Void      Zero( Void );
  Void      Identity( Void );
  Bool      IsIdentity( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsSymmetric( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsDiagonal( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsRotated( Void ) const;

  Void      ProjectVector( const Vec3 &src, Vec3 &dst ) const;
  Void      UnprojectVector( const Vec3 &src, Vec3 &dst ) const;

  Bool      FixDegeneracies( Void );  // fix degenerate axial cases
  Bool      FixDenormals( Void );    // change tiny numbers to zero

  Float     Trace( Void ) const;
  Float     Determinant( Void ) const;
  Mat3      OrthoNormalize( Void ) const;
  Mat3 &    OrthoNormalizeSelf( Void );
  Mat3      Transpose( Void ) const;  // returns transpose
  Mat3 &    TransposeSelf( Void );
  Mat3      Inverse( Void ) const;    // returns the inverse ( m * m.Inverse() = identity )
  Bool      InverseSelf( Void );    // returns false if determinant is zero
  Mat3      InverseFast( Void ) const;  // returns the inverse ( m * m.Inverse() = identity )
  Bool      InverseFastSelf( Void );  // returns false if determinant is zero
  Mat3      TransposeMultiply( const Mat3 &b ) const;

  Mat3      InertiaTranslate( const Float mass, const Vec3 &centerOfMass, const Vec3 &translation ) const;
  Mat3 &    InertiaTranslateSelf( const Float mass, const Vec3 &centerOfMass, const Vec3 &translation );
  Mat3      InertiaRotate( const Mat3 &rotation ) const;
  Mat3 &    InertiaRotateSelf( const Mat3 &rotation );

  Int       GetDimension( Void ) const;

  Angles    ToAngles( Void ) const;
  //Quat      ToQuat( Void ) const;
  //CQuat      ToCQuat( Void ) const;
  //Rotation    ToRotation( Void ) const;
  Mat4      ToMat4( Void ) const;
  Vec3      ToAngularVelocity( Void ) const;
  const Float *  ToFloatPtr( Void ) const;
  Float *      ToFloatPtr( Void );

  friend Void    TransposeMultiply( const Mat3 &inv, const Mat3 &b, Mat3 &dst );
  friend Mat3  SkewSymmetric( Vec3 const &src );

private:
  Vec3      mat[ 3 ];
};

extern Mat3 mat3_zero;
extern Mat3 mat3_identity;
#define mat3_default  mat3_identity

KM_INLINE Mat3::Mat3( Void ) {
}

KM_INLINE Mat3::Mat3( const Vec3 &x, const Vec3 &y, const Vec3 &z ) {
  mat[ 0 ].x = x.x; mat[ 0 ].y = x.y; mat[ 0 ].z = x.z;
  mat[ 1 ].x = y.x; mat[ 1 ].y = y.y; mat[ 1 ].z = y.z;
  mat[ 2 ].x = z.x; mat[ 2 ].y = z.y; mat[ 2 ].z = z.z;
}

KM_INLINE Mat3::Mat3( const Float xx, const Float xy, const Float xz, const Float yx, const Float yy, const Float yz, const Float zx, const Float zy, const Float zz ) {
  mat[ 0 ].x = xx; mat[ 0 ].y = xy; mat[ 0 ].z = xz;
  mat[ 1 ].x = yx; mat[ 1 ].y = yy; mat[ 1 ].z = yz;
  mat[ 2 ].x = zx; mat[ 2 ].y = zy; mat[ 2 ].z = zz;
}

KM_INLINE Mat3::Mat3( const Float src[ 3 ][ 3 ] ) {
  memcpy( mat, src, 3 * 3 * sizeof( Float ) );
}

KM_INLINE const Vec3 &Mat3::operator[]( Int index ) const {
  assert( ( index >= 0 ) && ( index < 3 ) );
  return mat[ index ];
}

KM_INLINE Vec3 &Mat3::operator[]( Int index ) {
  assert( ( index >= 0 ) && ( index < 3 ) );
  return mat[ index ];
}

KM_INLINE Mat3 Mat3::operator-() const {
  return Mat3(  -mat[0][0], -mat[0][1], -mat[0][2],
          -mat[1][0], -mat[1][1], -mat[1][2],
          -mat[2][0], -mat[2][1], -mat[2][2] );
}

KM_INLINE Vec3 Mat3::operator*( const Vec3 &vec ) const {
  return Vec3(
    mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z,
    mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z,
    mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z );
}

KM_INLINE Mat3 Mat3::operator*( const Mat3 &a ) const {
  Int i, j;
  const Float *m1Ptr, *m2Ptr;
  Float *dstPtr;
  Mat3 dst;

  m1Ptr = reinterpret_cast<const Float *>(this);
  m2Ptr = reinterpret_cast<const Float *>(&a);
  dstPtr = reinterpret_cast<Float *>(&dst);

  for ( i = 0; i < 3; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      *dstPtr = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
          + m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
          + m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
      dstPtr++;
    }
    m1Ptr += 3;
  }
  return dst;
}

KM_INLINE Mat3 Mat3::operator*( const Float a ) const {
  return Mat3(
    mat[0].x * a, mat[0].y * a, mat[0].z * a,
    mat[1].x * a, mat[1].y * a, mat[1].z * a,
    mat[2].x * a, mat[2].y * a, mat[2].z * a );
}

KM_INLINE Mat3 Mat3::operator+( const Mat3 &a ) const {
  return Mat3(
    mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z,
    mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z,
    mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z );
}
    
KM_INLINE Mat3 Mat3::operator-( const Mat3 &a ) const {
  return Mat3(
    mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z,
    mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z,
    mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z );
}

KM_INLINE Mat3 &Mat3::operator*=( const Float a ) {
  mat[0].x *= a; mat[0].y *= a; mat[0].z *= a;
  mat[1].x *= a; mat[1].y *= a; mat[1].z *= a; 
  mat[2].x *= a; mat[2].y *= a; mat[2].z *= a;

    return *this;
}

KM_INLINE Mat3 &Mat3::operator*=( const Mat3 &a ) {
  Int i, j;
  const Float *m2Ptr;
  Float *m1Ptr, dst[3];

  m1Ptr = reinterpret_cast<Float *>(this);
  m2Ptr = reinterpret_cast<const Float *>(&a);

  for ( i = 0; i < 3; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      dst[j]  = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
          + m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
          + m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
    }
    m1Ptr[0] = dst[0]; m1Ptr[1] = dst[1]; m1Ptr[2] = dst[2];
    m1Ptr += 3;
  }
  return *this;
}

KM_INLINE Mat3 &Mat3::operator+=( const Mat3 &a ) {
  mat[0].x += a[0].x; mat[0].y += a[0].y; mat[0].z += a[0].z;
  mat[1].x += a[1].x; mat[1].y += a[1].y; mat[1].z += a[1].z;
  mat[2].x += a[2].x; mat[2].y += a[2].y; mat[2].z += a[2].z;

    return *this;
}

KM_INLINE Mat3 &Mat3::operator-=( const Mat3 &a ) {
  mat[0].x -= a[0].x; mat[0].y -= a[0].y; mat[0].z -= a[0].z;
  mat[1].x -= a[1].x; mat[1].y -= a[1].y; mat[1].z -= a[1].z;
  mat[2].x -= a[2].x; mat[2].y -= a[2].y; mat[2].z -= a[2].z;

    return *this;
}

KM_INLINE Vec3 operator*( const Vec3 &vec, const Mat3 &mat ) {
  return mat * vec;
}

KM_INLINE Mat3 operator*( const Float a, const Mat3 &mat ) {
  return mat * a;
}

KM_INLINE Vec3 &operator*=( Vec3 &vec, const Mat3 &mat ) {
  Float x = mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z;
  Float y = mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z;
  vec.z = mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z;
  vec.x = x;
  vec.y = y;
  return vec;
}

KM_INLINE Bool Mat3::Compare( const Mat3 &a ) const {
  if ( mat[0].Compare( a[0] ) &&
    mat[1].Compare( a[1] ) &&
    mat[2].Compare( a[2] ) ) {
    return true;
  }
  return false;
}

KM_INLINE Bool Mat3::Compare( const Mat3 &a, const Float epsilon ) const {
  if ( mat[0].Compare( a[0], epsilon ) &&
    mat[1].Compare( a[1], epsilon ) &&
    mat[2].Compare( a[2], epsilon ) ) {
    return true;
  }
  return false;
}

KM_INLINE Bool Mat3::operator==( const Mat3 &a ) const {
  return Compare( a );
}

KM_INLINE Bool Mat3::operator!=( const Mat3 &a ) const {
  return !Compare( a );
}

KM_INLINE Void Mat3::Zero( Void ) {
  memset( mat, 0, sizeof( Mat3 ) );
}

KM_INLINE Void Mat3::Identity( Void ) {
  *this = mat3_identity;
}

KM_INLINE Bool Mat3::IsIdentity( const Float epsilon ) const {
  return Compare( mat3_identity, epsilon );
}

KM_INLINE Bool Mat3::IsSymmetric( const Float epsilon ) const {
  if ( Math::Fabs( mat[0][1] - mat[1][0] ) > epsilon ) {
    return false;
  }
  if ( Math::Fabs( mat[0][2] - mat[2][0] ) > epsilon ) {
    return false;
  }
  if ( Math::Fabs( mat[1][2] - mat[2][1] ) > epsilon ) {
    return false;
  }
  return true;
}

KM_INLINE Bool Mat3::IsDiagonal( const Float epsilon ) const {
  if ( Math::Fabs( mat[0][1] ) > epsilon ||
    Math::Fabs( mat[0][2] ) > epsilon ||
    Math::Fabs( mat[1][0] ) > epsilon ||
    Math::Fabs( mat[1][2] ) > epsilon ||
    Math::Fabs( mat[2][0] ) > epsilon ||
    Math::Fabs( mat[2][1] ) > epsilon ) {
    return false;
  }
  return true;
}

KM_INLINE Bool Mat3::IsRotated( Void ) const {
  return !Compare( mat3_identity );
}

KM_INLINE Void Mat3::ProjectVector( const Vec3 &src, Vec3 &dst ) const {
  dst.x = src * mat[ 0 ];
  dst.y = src * mat[ 1 ];
  dst.z = src * mat[ 2 ];
}

KM_INLINE Void Mat3::UnprojectVector( const Vec3 &src, Vec3 &dst ) const {
  dst = mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z;
}

KM_INLINE Bool Mat3::FixDegeneracies( Void ) {
  Bool r = mat[0].FixDegenerateNormal();
  r |= mat[1].FixDegenerateNormal();
  r |= mat[2].FixDegenerateNormal();
  return r;
}

KM_INLINE Bool Mat3::FixDenormals( Void ) {
  Bool r = mat[0].FixDenormals();
  r |= mat[1].FixDenormals();
  r |= mat[2].FixDenormals();
  return r;
}

KM_INLINE Float Mat3::Trace( Void ) const {
  return ( mat[0][0] + mat[1][1] + mat[2][2] );
}

KM_INLINE Mat3 Mat3::OrthoNormalize( Void ) const {
  Mat3 ortho;

  ortho = *this;
  ortho[ 0 ].Normalize();
  ortho[ 2 ].Cross( mat[ 0 ], mat[ 1 ] );
  ortho[ 2 ].Normalize();
  ortho[ 1 ].Cross( mat[ 2 ], mat[ 0 ] );
  ortho[ 1 ].Normalize();
  return ortho;
}

KM_INLINE Mat3 &Mat3::OrthoNormalizeSelf( Void ) {
  mat[ 0 ].Normalize();
  mat[ 2 ].Cross( mat[ 0 ], mat[ 1 ] );
  mat[ 2 ].Normalize();
  mat[ 1 ].Cross( mat[ 2 ], mat[ 0 ] );
  mat[ 1 ].Normalize();
  return *this;
}

KM_INLINE Mat3 Mat3::Transpose( Void ) const {
  return Mat3(  mat[0][0], mat[1][0], mat[2][0],
          mat[0][1], mat[1][1], mat[2][1],
          mat[0][2], mat[1][2], mat[2][2] );
}

KM_INLINE Mat3 &Mat3::TransposeSelf( Void ) {
  Float tmp0, tmp1, tmp2;

  tmp0 = mat[0][1];
  mat[0][1] = mat[1][0];
  mat[1][0] = tmp0;
  tmp1 = mat[0][2];
  mat[0][2] = mat[2][0];
  mat[2][0] = tmp1;
  tmp2 = mat[1][2];
  mat[1][2] = mat[2][1];
  mat[2][1] = tmp2;

  return *this;
}

KM_INLINE Mat3 Mat3::Inverse( Void ) const {
  Mat3 invMat;

  invMat = *this;
  Int r = invMat.InverseSelf();
  assert( r );
  return invMat;
}

KM_INLINE Mat3 Mat3::InverseFast( Void ) const {
  Mat3 invMat;

  invMat = *this;
  Int r = invMat.InverseFastSelf();
  assert( r );
  return invMat;
}

KM_INLINE Mat3 Mat3::TransposeMultiply( const Mat3 &b ) const {
  return Mat3(  mat[0].x * b[0].x + mat[1].x * b[1].x + mat[2].x * b[2].x,
          mat[0].x * b[0].y + mat[1].x * b[1].y + mat[2].x * b[2].y,
          mat[0].x * b[0].z + mat[1].x * b[1].z + mat[2].x * b[2].z,
          mat[0].y * b[0].x + mat[1].y * b[1].x + mat[2].y * b[2].x,
          mat[0].y * b[0].y + mat[1].y * b[1].y + mat[2].y * b[2].y,
          mat[0].y * b[0].z + mat[1].y * b[1].z + mat[2].y * b[2].z,
          mat[0].z * b[0].x + mat[1].z * b[1].x + mat[2].z * b[2].x,
          mat[0].z * b[0].y + mat[1].z * b[1].y + mat[2].z * b[2].y,
          mat[0].z * b[0].z + mat[1].z * b[1].z + mat[2].z * b[2].z );
}

KM_INLINE Void TransposeMultiply( const Mat3 &transpose, const Mat3 &b, Mat3 &dst ) {
  dst[0].x = transpose[0].x * b[0].x + transpose[1].x * b[1].x + transpose[2].x * b[2].x;
  dst[0].y = transpose[0].x * b[0].y + transpose[1].x * b[1].y + transpose[2].x * b[2].y;
  dst[0].z = transpose[0].x * b[0].z + transpose[1].x * b[1].z + transpose[2].x * b[2].z;
  dst[1].x = transpose[0].y * b[0].x + transpose[1].y * b[1].x + transpose[2].y * b[2].x;
  dst[1].y = transpose[0].y * b[0].y + transpose[1].y * b[1].y + transpose[2].y * b[2].y;
  dst[1].z = transpose[0].y * b[0].z + transpose[1].y * b[1].z + transpose[2].y * b[2].z;
  dst[2].x = transpose[0].z * b[0].x + transpose[1].z * b[1].x + transpose[2].z * b[2].x;
  dst[2].y = transpose[0].z * b[0].y + transpose[1].z * b[1].y + transpose[2].z * b[2].y;
  dst[2].z = transpose[0].z * b[0].z + transpose[1].z * b[1].z + transpose[2].z * b[2].z;
}

KM_INLINE Mat3 SkewSymmetric( Vec3 const &src ) {
  return Mat3( 0.0f, -src.z,  src.y, src.z,   0.0f, -src.x, -src.y,  src.x,   0.0f );
}

KM_INLINE Int Mat3::GetDimension( Void ) const {
  return 9;
}

KM_INLINE const Float *Mat3::ToFloatPtr( Void ) const {
  return mat[0].ToFloatPtr();
}

KM_INLINE Float *Mat3::ToFloatPtr( Void ) {
  return mat[0].ToFloatPtr();
}


//===============================================================
//
//  Mat4 - 4x4 matrix
//
//===============================================================

class Mat4 {
public:
  Mat4( Void );
  explicit Mat4( const Vec4 &x, const Vec4 &y, const Vec4 &z, const Vec4 &w );
  explicit Mat4( const Float xx, const Float xy, const Float xz, const Float xw,
                 const Float yx, const Float yy, const Float yz, const Float yw,
                 const Float zx, const Float zy, const Float zz, const Float zw,
                 const Float wx, const Float wy, const Float wz, const Float ww );
  explicit Mat4( const Mat3 &rotation, const Vec3 &translation );
  explicit Mat4( const Float src[ 4 ][ 4 ] );

  const Vec4 &  operator[]( Int index ) const;
  Vec4 &    operator[]( Int index );
  Mat4      operator*( const Float a ) const;
  Vec4      operator*( const Vec4 &vec ) const;
  Vec3      operator*( const Vec3 &vec ) const;
  Mat4      operator*( const Mat4 &a ) const;
  Mat4      operator+( const Mat4 &a ) const;
  Mat4      operator-( const Mat4 &a ) const;
  Mat4 &    operator*=( const Float a );
  Mat4 &    operator*=( const Mat4 &a );
  Mat4 &    operator+=( const Mat4 &a );
  Mat4 &    operator-=( const Mat4 &a );

  friend Mat4  operator*( const Float a, const Mat4 &mat );
  friend Vec4  operator*( const Vec4 &vec, const Mat4 &mat );
  friend Vec3  operator*( const Vec3 &vec, const Mat4 &mat );
  friend Vec4 &operator*=( Vec4 &vec, const Mat4 &mat );
  friend Vec3 &operator*=( Vec3 &vec, const Mat4 &mat );

  Bool      Compare( const Mat4 &a ) const;            // exact compare, no epsilon
  Bool      Compare( const Mat4 &a, const Float epsilon ) const;  // compare with epsilon
  Bool      operator==( const Mat4 &a ) const;          // exact compare, no epsilon
  Bool      operator!=( const Mat4 &a ) const;          // exact compare, no epsilon

  Void      Zero( Void );
  Void      Identity( Void );
  Bool      IsIdentity( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsSymmetric( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsDiagonal( const Float epsilon = MATRIX_EPSILON ) const;
  Bool      IsRotated( Void ) const;

  Void      ProjectVector( const Vec4 &src, Vec4 &dst ) const;
  Void      UnprojectVector( const Vec4 &src, Vec4 &dst ) const;

  Float     Trace( Void ) const;
  Float     Determinant( Void ) const;
  Mat4      Transpose( Void ) const;  // returns transpose
  Mat4 &    TransposeSelf( Void );
  Mat4      Inverse( Void ) const;    // returns the inverse ( m * m.Inverse() = identity )
  Bool      InverseSelf( Void );    // returns false if determinant is zero
  Mat4      InverseFast( Void ) const;  // returns the inverse ( m * m.Inverse() = identity )
  Bool      InverseFastSelf( Void );  // returns false if determinant is zero
  Mat4      TransposeMultiply( const Mat4 &b ) const;

  Int       GetDimension( Void ) const;

  const Float *  ToFloatPtr( Void ) const;
  Float *   ToFloatPtr( Void );

private:
  Vec4      mat[ 4 ];
};

extern Mat4 mat4_zero;
extern Mat4 mat4_identity;
#define mat4_default  mat4_identity

KM_INLINE Mat4::Mat4( Void ) {
}

KM_INLINE Mat4::Mat4( const Vec4 &x, const Vec4 &y, const Vec4 &z, const Vec4 &w ) {
  mat[ 0 ] = x;
  mat[ 1 ] = y;
  mat[ 2 ] = z;
  mat[ 3 ] = w;
}

KM_INLINE Mat4::Mat4( const Float xx, const Float xy, const Float xz, const Float xw,
              const Float yx, const Float yy, const Float yz, const Float yw,
              const Float zx, const Float zy, const Float zz, const Float zw,
              const Float wx, const Float wy, const Float wz, const Float ww ) {
  mat[0][0] = xx; mat[0][1] = xy; mat[0][2] = xz; mat[0][3] = xw;
  mat[1][0] = yx; mat[1][1] = yy; mat[1][2] = yz; mat[1][3] = yw;
  mat[2][0] = zx; mat[2][1] = zy; mat[2][2] = zz; mat[2][3] = zw;
  mat[3][0] = wx; mat[3][1] = wy; mat[3][2] = wz; mat[3][3] = ww;
}

KM_INLINE Mat4::Mat4( const Mat3 &rotation, const Vec3 &translation ) {
  // NOTE: Mat3 is transposed because it is column-major
  mat[ 0 ][ 0 ] = rotation[0][0];
  mat[ 0 ][ 1 ] = rotation[1][0];
  mat[ 0 ][ 2 ] = rotation[2][0];
  mat[ 0 ][ 3 ] = translation[0];
  mat[ 1 ][ 0 ] = rotation[0][1];
  mat[ 1 ][ 1 ] = rotation[1][1];
  mat[ 1 ][ 2 ] = rotation[2][1];
  mat[ 1 ][ 3 ] = translation[1];
  mat[ 2 ][ 0 ] = rotation[0][2];
  mat[ 2 ][ 1 ] = rotation[1][2];
  mat[ 2 ][ 2 ] = rotation[2][2];
  mat[ 2 ][ 3 ] = translation[2];
  mat[ 3 ][ 0 ] = 0.0f;
  mat[ 3 ][ 1 ] = 0.0f;
  mat[ 3 ][ 2 ] = 0.0f;
  mat[ 3 ][ 3 ] = 1.0f;
}

KM_INLINE Mat4::Mat4( const Float src[ 4 ][ 4 ] ) {
  memcpy( mat, src, 4 * 4 * sizeof( Float ) );
}

KM_INLINE const Vec4 &Mat4::operator[]( Int index ) const {
  assert( ( index >= 0 ) && ( index < 4 ) );
  return mat[ index ];
}

KM_INLINE Vec4 &Mat4::operator[]( Int index ) {
  assert( ( index >= 0 ) && ( index < 4 ) );
  return mat[ index ];
}

KM_INLINE Mat4 Mat4::operator*( const Float a ) const {
  return Mat4(
    mat[0].x * a, mat[0].y * a, mat[0].z * a, mat[0].w * a,
    mat[1].x * a, mat[1].y * a, mat[1].z * a, mat[1].w * a,
    mat[2].x * a, mat[2].y * a, mat[2].z * a, mat[2].w * a,
    mat[3].x * a, mat[3].y * a, mat[3].z * a, mat[3].w * a );
}

KM_INLINE Vec4 Mat4::operator*( const Vec4 &vec ) const {
  return Vec4(
    mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w * vec.w,
    mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w * vec.w,
    mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w * vec.w,
    mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w * vec.w );
}

KM_INLINE Vec3 Mat4::operator*( const Vec3 &vec ) const {
  Float s = mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w;
  if ( s == 0.0f ) {
    return Vec3( 0.0f, 0.0f, 0.0f );
  }
  if ( s == 1.0f ) {
    return Vec3(
      mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w,
      mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w,
      mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w );
  }
  else {
    Float invS = 1.0f / s;
    return Vec3(
      (mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w) * invS,
      (mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w) * invS,
      (mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w) * invS );
  }
}

KM_INLINE Mat4 Mat4::operator*( const Mat4 &a ) const {
  Int i, j;
  const Float *m1Ptr, *m2Ptr;
  Float *dstPtr;
  Mat4 dst;

  m1Ptr = reinterpret_cast<const Float *>(this);
  m2Ptr = reinterpret_cast<const Float *>(&a);
  dstPtr = reinterpret_cast<Float *>(&dst);

  for ( i = 0; i < 4; i++ ) {
    for ( j = 0; j < 4; j++ ) {
      *dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
          + m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
          + m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
          + m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
      dstPtr++;
    }
    m1Ptr += 4;
  }
  return dst;
}

KM_INLINE Mat4 Mat4::operator+( const Mat4 &a ) const {
  return Mat4( 
    mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z, mat[0].w + a[0].w,
    mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z, mat[1].w + a[1].w,
    mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z, mat[2].w + a[2].w,
    mat[3].x + a[3].x, mat[3].y + a[3].y, mat[3].z + a[3].z, mat[3].w + a[3].w );
}
    
KM_INLINE Mat4 Mat4::operator-( const Mat4 &a ) const {
  return Mat4( 
    mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z, mat[0].w - a[0].w,
    mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z, mat[1].w - a[1].w,
    mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z, mat[2].w - a[2].w,
    mat[3].x - a[3].x, mat[3].y - a[3].y, mat[3].z - a[3].z, mat[3].w - a[3].w );
}

KM_INLINE Mat4 &Mat4::operator*=( const Float a ) {
  mat[0].x *= a; mat[0].y *= a; mat[0].z *= a; mat[0].w *= a;
  mat[1].x *= a; mat[1].y *= a; mat[1].z *= a; mat[1].w *= a;
  mat[2].x *= a; mat[2].y *= a; mat[2].z *= a; mat[2].w *= a;
  mat[3].x *= a; mat[3].y *= a; mat[3].z *= a; mat[3].w *= a;
    return *this;
}

KM_INLINE Mat4 &Mat4::operator*=( const Mat4 &a ) {
  *this = (*this) * a;
  return *this;
}

KM_INLINE Mat4 &Mat4::operator+=( const Mat4 &a ) {
  mat[0].x += a[0].x; mat[0].y += a[0].y; mat[0].z += a[0].z; mat[0].w += a[0].w;
  mat[1].x += a[1].x; mat[1].y += a[1].y; mat[1].z += a[1].z; mat[1].w += a[1].w;
  mat[2].x += a[2].x; mat[2].y += a[2].y; mat[2].z += a[2].z; mat[2].w += a[2].w;
  mat[3].x += a[3].x; mat[3].y += a[3].y; mat[3].z += a[3].z; mat[3].w += a[3].w;
    return *this;
}

KM_INLINE Mat4 &Mat4::operator-=( const Mat4 &a ) {
  mat[0].x -= a[0].x; mat[0].y -= a[0].y; mat[0].z -= a[0].z; mat[0].w -= a[0].w;
  mat[1].x -= a[1].x; mat[1].y -= a[1].y; mat[1].z -= a[1].z; mat[1].w -= a[1].w;
  mat[2].x -= a[2].x; mat[2].y -= a[2].y; mat[2].z -= a[2].z; mat[2].w -= a[2].w;
  mat[3].x -= a[3].x; mat[3].y -= a[3].y; mat[3].z -= a[3].z; mat[3].w -= a[3].w;
    return *this;
}

KM_INLINE Mat4 operator*( const Float a, const Mat4 &mat ) {
  return mat * a;
}

KM_INLINE Vec4 operator*( const Vec4 &vec, const Mat4 &mat ) {
  return mat * vec;
}

KM_INLINE Vec3 operator*( const Vec3 &vec, const Mat4 &mat ) {
  return mat * vec;
}

KM_INLINE Vec4 &operator*=( Vec4 &vec, const Mat4 &mat ) {
  vec = mat * vec;
  return vec;
}

KM_INLINE Vec3 &operator*=( Vec3 &vec, const Mat4 &mat ) {
  vec = mat * vec;
  return vec;
}

KM_INLINE Bool Mat4::Compare( const Mat4 &a ) const {
  Dword i;
  const Float *ptr1, *ptr2;

  ptr1 = reinterpret_cast<const Float *>(mat);
  ptr2 = reinterpret_cast<const Float *>(a.mat);
  for ( i = 0; i < 4*4; i++ ) {
    if ( ptr1[i] != ptr2[i] ) {
      return false;
    }
  }
  return true;
}

KM_INLINE Bool Mat4::Compare( const Mat4 &a, const Float epsilon ) const {
  Dword i;
  const Float *ptr1, *ptr2;

  ptr1 = reinterpret_cast<const Float *>(mat);
  ptr2 = reinterpret_cast<const Float *>(a.mat);
  for ( i = 0; i < 4*4; i++ ) {
    if ( Math::Fabs( ptr1[i] - ptr2[i] ) > epsilon ) {
      return false;
    }
  }
  return true;
}

KM_INLINE Bool Mat4::operator==( const Mat4 &a ) const {
  return Compare( a );
}

KM_INLINE Bool Mat4::operator!=( const Mat4 &a ) const {
  return !Compare( a );
}

KM_INLINE Void Mat4::Zero( Void ) {
  memset( mat, 0, sizeof( Mat4 ) );
}

KM_INLINE Void Mat4::Identity( Void ) {
  *this = mat4_identity;
}

KM_INLINE Bool Mat4::IsIdentity( const Float epsilon ) const {
  return Compare( mat4_identity, epsilon );
}

KM_INLINE Bool Mat4::IsSymmetric( const Float epsilon ) const {
  for ( Int i = 1; i < 4; i++ ) {
    for ( Int j = 0; j < i; j++ ) {
      if ( Math::Fabs( mat[i][j] - mat[j][i] ) > epsilon ) {
        return false;
      }
    }
  }
  return true;
}

KM_INLINE Bool Mat4::IsDiagonal( const Float epsilon ) const {
  for ( Int i = 0; i < 4; i++ ) {
    for ( Int j = 0; j < 4; j++ ) {
      if ( i != j && Math::Fabs( mat[i][j] ) > epsilon ) {
        return false;
      }
    }
  }
  return true;
}

KM_INLINE Bool Mat4::IsRotated( Void ) const {
  if ( !mat[ 0 ][ 1 ] && !mat[ 0 ][ 2 ] &&
    !mat[ 1 ][ 0 ] && !mat[ 1 ][ 2 ] &&
    !mat[ 2 ][ 0 ] && !mat[ 2 ][ 1 ] ) {
    return false;
  }
  return true;
}

KM_INLINE Void Mat4::ProjectVector( const Vec4 &src, Vec4 &dst ) const {
  dst.x = src * mat[ 0 ];
  dst.y = src * mat[ 1 ];
  dst.z = src * mat[ 2 ];
  dst.w = src * mat[ 3 ];
}

KM_INLINE Void Mat4::UnprojectVector( const Vec4 &src, Vec4 &dst ) const {
  dst = mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z + mat[ 3 ] * src.w;
}

KM_INLINE Float Mat4::Trace( Void ) const {
  return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] );
}

KM_INLINE Mat4 Mat4::Inverse( Void ) const {
  Mat4 invMat;

  invMat = *this;
  Int r = invMat.InverseSelf();
  assert( r );
  return invMat;
}

KM_INLINE Mat4 Mat4::InverseFast( Void ) const {
  Mat4 invMat;

  invMat = *this;
  Int r = invMat.InverseFastSelf();
  assert( r );
  return invMat;
}

KM_INLINE Mat4 Mat3::ToMat4( Void ) const {
  // NOTE: Mat3 is transposed because it is column-major
  return Mat4(  mat[0][0],  mat[1][0],  mat[2][0],  0.0f,
          mat[0][1],  mat[1][1],  mat[2][1],  0.0f,
          mat[0][2],  mat[1][2],  mat[2][2],  0.0f,
          0.0f,       0.0f,       0.0f,       1.0f );
}

KM_INLINE Int Mat4::GetDimension( Void ) const {
  return 16;
}

KM_INLINE const Float *Mat4::ToFloatPtr( Void ) const {
  return mat[0].ToFloatPtr();
}

KM_INLINE Float *Mat4::ToFloatPtr( Void ) {
  return mat[0].ToFloatPtr();
}
