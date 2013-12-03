#pragma once

#include "klib.h"

class Vec3;
class Quat;
class Rotation;
class Mat3;
class Mat4;

class Angles {
public:
  Float     pitch;
  Float     yaw;
  Float     roll;

  Angles( Void );
  Angles( Float pitch, Float yaw, Float roll );
  explicit Angles( const Vec3 &v );

  Void      Set( Float pitch, Float yaw, Float roll );
  Angles &  Zero( Void );

  Float     operator[]( Int index ) const;
  Float &   operator[]( Int index );
  Angles    operator-() const;      // negate angles, in general not the inverse rotation
  Angles &  operator=( const Angles &a );
  Angles    operator+( const Angles &a ) const;
  Angles &  operator+=( const Angles &a );
  Angles    operator-( const Angles &a ) const;
  Angles &  operator-=( const Angles &a );
  Angles    operator*( const Float a ) const;
  Angles &  operator*=( const Float a );
  Angles    operator/( const Float a ) const;
  Angles &  operator/=( const Float a );

  friend Angles  operator*( const Float a, const Angles &b );

  Bool      Compare( const Angles &a ) const;              // exact compare, no epsilon
  Bool      Compare( const Angles &a, const Float epsilon ) const;  // compare with epsilon
  Bool      operator==(  const Angles &a ) const;            // exact compare, no epsilon
  Bool      operator!=(  const Angles &a ) const;            // exact compare, no epsilon

  Angles &  Normalize360( Void );  // normalizes 'this'
  Angles &  Normalize180( Void );  // normalizes 'this'

  Void      Clamp( const Angles &min, const Angles &max );

  Int       GetDimension( Void ) const;

  Void      ToVectors( Vec3 *forward, Vec3 *right = NULL, Vec3 *up = NULL ) const;
  Vec3      ToForward( Void ) const;
  Quat      ToQuat( Void ) const;
  Rotation  ToRotation( Void ) const;
  Mat3      ToMat3( Void ) const;
  Mat4      ToMat4( Void ) const;
  Vec3      ToAngularVelocity( Void ) const;
  const Float *  ToFloatPtr( Void ) const;
  Float *   ToFloatPtr( Void );
};

extern Angles ang_zero;

KM_INLINE Angles::Angles( Void ) {
}

KM_INLINE Angles::Angles( Float pitch, Float yaw, Float roll ) {
  this->pitch = pitch;
  this->yaw   = yaw;
  this->roll  = roll;
}

KM_INLINE Angles::Angles( const Vec3 &v ) {
  this->pitch = v[0];
  this->yaw   = v[1];
  this->roll  = v[2];
}

KM_INLINE Void Angles::Set( Float pitch, Float yaw, Float roll ) {
  this->pitch = pitch;
  this->yaw   = yaw;
  this->roll  = roll;
}

KM_INLINE Angles &Angles::Zero( Void ) {
  pitch = yaw = roll = 0.0f;
  return *this;
}

KM_INLINE Float Angles::operator[]( Int index ) const {
  assert( ( index >= 0 ) && ( index < 3 ) );
  return ( &pitch )[ index ];
}

KM_INLINE Float &Angles::operator[]( Int index ) {
  assert( ( index >= 0 ) && ( index < 3 ) );
  return ( &pitch )[ index ];
}

KM_INLINE Angles Angles::operator-() const {
  return Angles( -pitch, -yaw, -roll );
}

KM_INLINE Angles &Angles::operator=( const Angles &a ) {
  pitch = a.pitch;
  yaw   = a.yaw;
  roll  = a.roll;
  return *this;
}

KM_INLINE Angles Angles::operator+( const Angles &a ) const {
  return Angles( pitch + a.pitch, yaw + a.yaw, roll + a.roll );
}

KM_INLINE Angles& Angles::operator+=( const Angles &a ) {
  pitch += a.pitch;
  yaw   += a.yaw;
  roll  += a.roll;

  return *this;
}

KM_INLINE Angles Angles::operator-( const Angles &a ) const {
  return Angles( pitch - a.pitch, yaw - a.yaw, roll - a.roll );
}

KM_INLINE Angles& Angles::operator-=( const Angles &a ) {
  pitch -= a.pitch;
  yaw   -= a.yaw;
  roll  -= a.roll;

  return *this;
}

KM_INLINE Angles Angles::operator*( const Float a ) const {
  return Angles( pitch * a, yaw * a, roll * a );
}

KM_INLINE Angles& Angles::operator*=( Float a ) {
  pitch *= a;
  yaw   *= a;
  roll  *= a;
  return *this;
}

KM_INLINE Angles Angles::operator/( const Float a ) const {
  Float inva = 1.0f / a;
  return Angles( pitch * inva, yaw * inva, roll * inva );
}

KM_INLINE Angles& Angles::operator/=( Float a ) {
  Float inva = 1.0f / a;
  pitch *= inva;
  yaw   *= inva;
  roll  *= inva;
  return *this;
}

KM_INLINE Angles operator*( const Float a, const Angles &b ) {
  return Angles( a * b.pitch, a * b.yaw, a * b.roll );
}

KM_INLINE Bool Angles::Compare( const Angles &a ) const {
  return ( ( a.pitch == pitch ) && ( a.yaw == yaw ) && ( a.roll == roll ) );
}

KM_INLINE Bool Angles::Compare( const Angles &a, const Float epsilon ) const {
  if ( Math::Fabs( pitch - a.pitch ) > epsilon ) {
    return false;
  }

  if ( Math::Fabs( yaw - a.yaw ) > epsilon ) {
    return false;
  }

  if ( Math::Fabs( roll - a.roll ) > epsilon ) {
    return false;
  }

  return true;
}

KM_INLINE Bool Angles::operator==( const Angles &a ) const {
  return Compare( a );
}

KM_INLINE Bool Angles::operator!=( const Angles &a ) const {
  return !Compare( a );
}

KM_INLINE Void Angles::Clamp( const Angles &min, const Angles &max ) {
  if ( pitch < min.pitch ) {
    pitch = min.pitch;
  } else if ( pitch > max.pitch ) {
    pitch = max.pitch;
  }
  if ( yaw < min.yaw ) {
    yaw = min.yaw;
  } else if ( yaw > max.yaw ) {
    yaw = max.yaw;
  }
  if ( roll < min.roll ) {
    roll = min.roll;
  } else if ( roll > max.roll ) {
    roll = max.roll;
  }
}

KM_INLINE Int Angles::GetDimension( Void ) const {
  return 3;
}

KM_INLINE const Float *Angles::ToFloatPtr( Void ) const {
  return &pitch;
}

KM_INLINE Float *Angles::ToFloatPtr( Void ) {
  return &pitch;
}
