#pragma once


#include "types.h"


class Vec3;
//class idAngles;
//class idRotation;
class Mat3;
class Mat4;
class idCQuat;

class Quat {
public:
	float			x;
	float			y;
	float			z;
	float			w;

					Quat( void );
					Quat( float x, float y, float z, float w );

	void 			Set( float x, float y, float z, float w );

	float			operator[]( int index ) const;
	float &			operator[]( int index );
	Quat			operator-() const;
	Quat &		operator=( const Quat &a );
	Quat			operator+( const Quat &a ) const;
	Quat &		operator+=( const Quat &a );
	Quat			operator-( const Quat &a ) const;
	Quat &		operator-=( const Quat &a );
	Quat			operator*( const Quat &a ) const;
	Vec3			operator*( const Vec3 &a ) const;
	Quat			operator*( float a ) const;
	Quat &		operator*=( const Quat &a );
	Quat &		operator*=( float a );

	friend Quat	operator*( const float a, const Quat &b );
	friend Vec3	operator*( const Vec3 &a, const Quat &b );

	bool			Compare( const Quat &a ) const;						// exact compare, no epsilon
	bool			Compare( const Quat &a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const Quat &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const Quat &a ) const;					// exact compare, no epsilon

	Quat			Inverse( void ) const;
	float			Length( void ) const;
	Quat &		Normalize( void );

	float			CalcW( void ) const;
	int				GetDimension( void ) const;

	//idAngles		ToAngles( void ) const;
	//idRotation		ToRotation( void ) const;
	Mat3			ToMat3( void ) const;
	Mat4			ToMat4( void ) const;
	idCQuat			ToCQuat( void ) const;
	Vec3			ToAngularVelocity( void ) const;
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	//const char *	ToString( int precision = 2 ) const;

	Quat &		Slerp( const Quat &from, const Quat &to, float t );
};

KM_INLINE Quat::Quat( void ) {
}

KM_INLINE Quat::Quat( float x, float y, float z, float w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

KM_INLINE float Quat::operator[]( int index ) const {
	assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

KM_INLINE float& Quat::operator[]( int index ) {
	assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

KM_INLINE Quat Quat::operator-() const {
	return Quat( -x, -y, -z, -w );
}

KM_INLINE Quat &Quat::operator=( const Quat &a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;

	return *this;
}

KM_INLINE Quat Quat::operator+( const Quat &a ) const {
	return Quat( x + a.x, y + a.y, z + a.z, w + a.w );
}

KM_INLINE Quat& Quat::operator+=( const Quat &a ) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

KM_INLINE Quat Quat::operator-( const Quat &a ) const {
	return Quat( x - a.x, y - a.y, z - a.z, w - a.w );
}

KM_INLINE Quat& Quat::operator-=( const Quat &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

KM_INLINE Quat Quat::operator*( const Quat &a ) const {
	return Quat(	w*a.x + x*a.w + y*a.z - z*a.y,
					w*a.y + y*a.w + z*a.x - x*a.z,
					w*a.z + z*a.w + x*a.y - y*a.x,
					w*a.w - x*a.x - y*a.y - z*a.z );
}

KM_INLINE Vec3 Quat::operator*( const Vec3 &a ) const {
#if 0
	// it's faster to do the conversion to a 3x3 matrix and multiply the vector by this 3x3 matrix
	return ( ToMat3() * a );
#else
	// result = this->Inverse() * Quat( a.x, a.y, a.z, 0.0f ) * (*this)
	float xxzz = x*x - z*z;
	float wwyy = w*w - y*y;

	float xw2 = x*w*2.0f;
	float xy2 = x*y*2.0f;
	float xz2 = x*z*2.0f;
	float yw2 = y*w*2.0f;
	float yz2 = y*z*2.0f;
	float zw2 = z*w*2.0f;

	return Vec3(
		(xxzz + wwyy)*a.x		+ (xy2 + zw2)*a.y		+ (xz2 - yw2)*a.z,
		(xy2 - zw2)*a.x			+ (y*y+w*w-x*x-z*z)*a.y	+ (yz2 + xw2)*a.z,
		(xz2 + yw2)*a.x			+ (yz2 - xw2)*a.y		+ (wwyy - xxzz)*a.z
	);
#endif
}

KM_INLINE Quat Quat::operator*( float a ) const {
	return Quat( x * a, y * a, z * a, w * a );
}

KM_INLINE Quat operator*( const float a, const Quat &b ) {
	return b * a;
}

KM_INLINE Vec3 operator*( const Vec3 &a, const Quat &b ) {
	return b * a;
}

KM_INLINE Quat& Quat::operator*=( const Quat &a ) {
	*this = *this * a;

	return *this;
}

KM_INLINE Quat& Quat::operator*=( float a ) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

KM_INLINE bool Quat::Compare( const Quat &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && ( w == a.w ) );
}

KM_INLINE bool Quat::Compare( const Quat &a, const float epsilon ) const {
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

KM_INLINE bool Quat::operator==( const Quat &a ) const {
	return Compare( a );
}

KM_INLINE bool Quat::operator!=( const Quat &a ) const {
	return !Compare( a );
}

KM_INLINE void Quat::Set( float x, float y, float z, float w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

KM_INLINE Quat Quat::Inverse( void ) const {
	return Quat( -x, -y, -z, w );
}

KM_INLINE float Quat::Length( void ) const {
	float len;

	len = x * x + y * y + z * z + w * w;
	return Math::Sqrt( len );
}

KM_INLINE Quat& Quat::Normalize( void ) {
	float len;
	float ilength;

	len = this->Length();
	if ( len ) {
		ilength = 1 / len;
		x *= ilength;
		y *= ilength;
		z *= ilength;
		w *= ilength;
	}
	return *this;
}

KM_INLINE float Quat::CalcW( void ) const {
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return sqrt( fabs( 1.0f - ( x * x + y * y + z * z ) ) );
}

KM_INLINE int Quat::GetDimension( void ) const {
	return 4;
}

KM_INLINE const float *Quat::ToFloatPtr( void ) const {
	return &x;
}

KM_INLINE float *Quat::ToFloatPtr( void ) {
	return &x;
}


/*
===============================================================================

	Compressed quaternion

===============================================================================
*/

class idCQuat {
public:
	float			x;
	float			y;
	float			z;

					idCQuat( void );
					idCQuat( float x, float y, float z );

	void 			Set( float x, float y, float z );

	float			operator[]( int index ) const;
	float &			operator[]( int index );

	bool			Compare( const idCQuat &a ) const;						// exact compare, no epsilon
	bool			Compare( const idCQuat &a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const idCQuat &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const idCQuat &a ) const;					// exact compare, no epsilon

	int				GetDimension( void ) const;

	//idAngles		ToAngles( void ) const;
	//idRotation		ToRotation( void ) const;
	Mat3			ToMat3( void ) const;
	Mat4			ToMat4( void ) const;
	Quat			ToQuat( void ) const;
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;
};

KM_INLINE idCQuat::idCQuat( void ) {
}

KM_INLINE idCQuat::idCQuat( float x, float y, float z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

KM_INLINE void idCQuat::Set( float x, float y, float z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

KM_INLINE float idCQuat::operator[]( int index ) const {
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

KM_INLINE float& idCQuat::operator[]( int index ) {
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

KM_INLINE bool idCQuat::Compare( const idCQuat &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

KM_INLINE bool idCQuat::Compare( const idCQuat &a, const float epsilon ) const {
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

KM_INLINE bool idCQuat::operator==( const idCQuat &a ) const {
	return Compare( a );
}

KM_INLINE bool idCQuat::operator!=( const idCQuat &a ) const {
	return !Compare( a );
}

KM_INLINE int idCQuat::GetDimension( void ) const {
	return 3;
}

KM_INLINE Quat idCQuat::ToQuat( void ) const {
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return Quat( x, y, z, sqrt( fabs( 1.0f - ( x * x + y * y + z * z ) ) ) );
}

KM_INLINE const float *idCQuat::ToFloatPtr( void ) const {
	return &x;
}

KM_INLINE float *idCQuat::ToFloatPtr( void ) {
	return &x;
}
