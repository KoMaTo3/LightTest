#pragma once

#include <math.h>
#include <stdio.h>
#include "types.h"
//#include "klib.h"

#ifndef assert
#define assert(a) if( !( a ) ) printf( "Fatal error: file[%s] line[%d]\n", __FILE__, __LINE__ );
#endif


#define DEG2RAD(a)            ( (a) * Math::M_DEG2RAD )
#define RAD2DEG(a)            ( (a) * Math::M_RAD2DEG )

#define FLOAT_IS_NAN(x)       (((*(const unsigned long *)&x) & 0x7f800000) == 0x7f800000)
#define FLOAT_IS_INF(x)       (((*(const unsigned long *)&x) & 0x7fffffff) == 0x7f800000)
#define FLOAT_IS_IND(x)       ((*(const unsigned long *)&x) == 0xffc00000)
#define FLOAT_IS_DENORMAL(x)  (((*(const unsigned long *)&x) & 0x7f800000) == 0x00000000 && \
                              ((*(const unsigned long *)&x) & 0x007fffff) != 0x00000000 )

#define FLOATSIGNBITSET(f)    ((*(const unsigned long *)&(f)) >> 31)
#define FLOATSIGNBITNOTSET(f) ((~(*(const unsigned long *)&(f))) >> 31)
#define FLOATNOTZERO(f)       ((*(const unsigned long *)&(f)) & ~(1<<31) )
#define INTSIGNBITSET(i)      (((const unsigned long)(i)) >> 31)
#define INTSIGNBITNOTSET(i)   ((~((const unsigned long)(i))) >> 31)

#define IEEE_FLT_MANTISSA_BITS  23
#define IEEE_FLT_EXPONENT_BITS  8
#define IEEE_FLT_EXPONENT_BIAS  127
#define IEEE_FLT_SIGN_BIT       31

template<class T> KM_INLINE Int  MaxIndex( T x, T y ) { return  ( x > y ) ? 0 : 1; }
template<class T> KM_INLINE Int  MinIndex( T x, T y ) { return ( x < y ) ? 0 : 1; }

template<class T> KM_INLINE T  Max3( T x, T y, T z ) { return ( x > y ) ? ( ( x > z ) ? x : z ) : ( ( y > z ) ? y : z ); }
template<class T> KM_INLINE T  Min3( T x, T y, T z ) { return ( x < y ) ? ( ( x < z ) ? x : z ) : ( ( y < z ) ? y : z ); }
template<class T> KM_INLINE Int  Max3Index( T x, T y, T z ) { return ( x > y ) ? ( ( x > z ) ? 0 : 2 ) : ( ( y > z ) ? 1 : 2 ); }
template<class T> KM_INLINE Int  Min3Index( T x, T y, T z ) { return ( x < y ) ? ( ( x < z ) ? 0 : 2 ) : ( ( y < z ) ? 1 : 2 ); }

template<class T> KM_INLINE T  Sign( T f ) { return ( f > 0 ) ? ( T ) 1 : ( ( f < 0 ) ? ( T ) -1 : 0 ); }
template<class T> KM_INLINE T  Square( T x ) { return x * x; }
template<class T> KM_INLINE T  Cube( T x ) { return x * x * x; }
template<class T> KM_INLINE T  Round( T x ) { return x < 0.0 ? ceil( x - 0.5f ) : floor( x + 0.5f ); }

class Math
{
public:
  static Void         Init( Void );

  static Float        RSqrt( Float x );      // reciprocal square root, returns huge number when x == 0.0
  template < class T >
  static void         Swap( T& a, T& b );   //swap values a and b
  template < class T >
  static bool         Between( T l, T r, T x, float epsilon );  //test  l-E <= x <= r+E
  template < class T >
  static bool Intersect1D( T a, T b, T c, T d, float epsilon = 0.01f ); //test on intersect [a; b] and [c; d]

  static Float        InvSqrt( Float x );      // inverse square root with 32 bits precision, returns huge number when x == 0.0
  static Float        InvSqrt16( Float x );    // inverse square root with 16 bits precision, returns huge number when x == 0.0
  static Double       InvSqrt64( Float x );    // inverse square root with 64 bits precision, returns huge number when x == 0.0

  static Float        Sqrt( Float x );      // square root with 32 bits precision
  static Float        Sqrt16( Float x );      // square root with 16 bits precision
  static Double       Sqrt64( Float x );      // square root with 64 bits precision

  static Float        Sin( Float a );        // sine with 32 bits precision
  static Float        Sin16( Float a );      // sine with 16 bits precision, maximum absolute error is 2.3082e-09
  static Double       Sin64( Float a );      // sine with 64 bits precision

  static Float        Cos( Float a );        // cosine with 32 bits precision
  static Float        Cos16( Float a );      // cosine with 16 bits precision, maximum absolute error is 2.3082e-09
  static Double       Cos64( Float a );      // cosine with 64 bits precision

  static Void         SinCos( Float a, Float &s, Float &c );    // sine and cosine with 32 bits precision
  static Void         SinCos16( Float a, Float &s, Float &c );  // sine and cosine with 16 bits precision
  static Void         SinCos64( Float a, Double &s, Double &c );  // sine and cosine with 64 bits precision

  static Float        Tan( Float a );        // tangent with 32 bits precision
  static Float        Tan16( Float a );      // tangent with 16 bits precision, maximum absolute error is 1.8897e-08
  static Double       Tan64( Float a );      // tangent with 64 bits precision

  static Float        ASin( Float a );      // arc sine with 32 bits precision, input is clamped to [-1, 1] to avoid a silent NaN
  static Float        ASin16( Float a );      // arc sine with 16 bits precision, maximum absolute error is 6.7626e-05
  static Double       ASin64( Float a );      // arc sine with 64 bits precision

  static Float        ACos( Float a );      // arc cosine with 32 bits precision, input is clamped to [-1, 1] to avoid a silent NaN
  static Float        ACos16( Float a );      // arc cosine with 16 bits precision, maximum absolute error is 6.7626e-05
  static Double       ACos64( Float a );      // arc cosine with 64 bits precision

  static Float        ATan( Float a );      // arc tangent with 32 bits precision
  static Float        ATan16( Float a );      // arc tangent with 16 bits precision, maximum absolute error is 1.3593e-08
  static Double       ATan64( Float a );      // arc tangent with 64 bits precision

  static Float        ATan( Float y, Float x );  // arc tangent with 32 bits precision
  static Float        ATan16( Float y, Float x );  // arc tangent with 16 bits precision, maximum absolute error is 1.3593e-08
  static Double       ATan64( Float y, Float x );  // arc tangent with 64 bits precision

  static Float        Pow( Float x, Float y );  // x raised to the power y with 32 bits precision
  static Float        Pow16( Float x, Float y );  // x raised to the power y with 16 bits precision
  static Double       Pow64( Float x, Float y );  // x raised to the power y with 64 bits precision

  static Float        Exp( Float f );        // e raised to the power f with 32 bits precision
  static Float        Exp16( Float f );      // e raised to the power f with 16 bits precision
  static Double       Exp64( Float f );      // e raised to the power f with 64 bits precision

  static Float        Log( Float f );        // natural logarithm with 32 bits precision
  static Float        Log16( Float f );      // natural logarithm with 16 bits precision
  static Double       Log64( Float f );      // natural logarithm with 64 bits precision

  static Int          IPow( Int x, Int y );    // integral x raised to the power y
  static Int          ILog2( Float f );      // integral base-2 logarithm of the floating point value
  static Int          ILog2( Int i );        // integral base-2 logarithm of the integer value

  static Int          Abs( Int x );        // returns the absolute value of the integer value (for reference only)
  static Float        Fabs( Float f );      // returns the absolute value of the floating point value
  static Float        Floor( Float f );      // returns the largest integer that is less than or equal to the given value
  static Float        Ceil( Float f );      // returns the smallest integer that is greater than or equal to the given value
  static Float        Rint( Float f );      // returns the nearest integer
  static Int          Ftoi( Float f );      // Float to Int conversion
  static Int          FtoiFast( Float f );    // fast Float to Int conversion but uses current FPU round mode (default round nearest)
  static unsigned long  Ftol( Float f );      // Float to Long conversion
  static unsigned long  FtolFast( Float );      // fast Float to Long conversion but uses current FPU round mode (default round nearest)

  static signed char  ClampChar( Int i );
  static signed short ClampShort( Int i );
  static Int          ClampInt( Int min, Int max, Int value );
  static Float        ClampFloat( Float min, Float max, Float value );

  static Float        AngleNormalize360( Float angle );
  static Float        AngleNormalize180( Float angle );
  static Float        AngleDelta( Float angle1, Float angle2 );

  static const Float      PI;             // pi
  static const Float      TWO_PI;         // pi * 2
  static const Float      HALF_PI;        // pi / 2
  static const Float      ONEFOURTH_PI;   // pi / 4
  static const Float      E;              // e
  static const Float      SQRT_TWO;       // sqrt( 2 )
  static const Float      SQRT_THREE;     // sqrt( 3 )
  static const Float      SQRT_1OVER2;    // sqrt( 1 / 2 )
  static const Float      SQRT_1OVER3;    // sqrt( 1 / 3 )
  static const Float      M_DEG2RAD;      // degrees to radians multiplier
  static const Float      M_RAD2DEG;      // radians to degrees multiplier
  static const Float      M_SEC2MS;       // seconds to milliseconds multiplier
  static const Float      M_MS2SEC;       // milliseconds to seconds multiplier
  static const Float      INFINITY;       // huge number which should be larger than any valid number used
  static const Float      FLT_EPSILON_NUM;    // smallest positive number such that 1.0+FLT_EPSILON != 1.0

private:
  union _flint {
    Dword           i;
    Float           f;
  };

  enum {
    LOOKUP_BITS     = 8,
    EXP_POS         = 23,
    EXP_BIAS        = 127,
    LOOKUP_POS      = (EXP_POS-LOOKUP_BITS),
    SEED_POS        = (EXP_POS-8),
    SQRT_TABLE_SIZE = (2<<LOOKUP_BITS),
    LOOKUP_MASK     = (SQRT_TABLE_SIZE-1)
  };

  static Dword      iSqrt[SQRT_TABLE_SIZE];
  static Bool       initialized;
};



KM_INLINE Float Math::RSqrt( Float x ) {

  Long i;
  Float y, r;

  y = x * 0.5f;
  i = *reinterpret_cast<Long *>( &x );
  i = 0x5f3759df - ( i >> 1 );
  r = *reinterpret_cast<Float *>( &i );
  r = r * ( 1.5f - r * r * y );
  return r;
}

KM_INLINE Float Math::InvSqrt16( Float x ) {
  assert( Math::initialized );
  Dword a = ((union _flint*)(&x))->i;
  union _flint seed;

  Double y = x * 0.5f;
  seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
  Double r = seed.f;
  r = r * ( 1.5f - r * r * y );
  return (Float) r;
}

KM_INLINE Float Math::InvSqrt( Float x ) {
  assert( Math::initialized );

  Dword a = ((union _flint*)(&x))->i;
  union _flint seed;

  Double y = x * 0.5f;
  seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
  Double r = seed.f;
  r = r * ( 1.5f - r * r * y );
  r = r * ( 1.5f - r * r * y );
  return (Float) r;
}

KM_INLINE Double Math::InvSqrt64( Float x ) {
  assert( Math::initialized );

  Dword a = ((union _flint*)(&x))->i;
  union _flint seed;

  Double y = x * 0.5f;
  seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
  Double r = seed.f;
  r = r * ( 1.5f - r * r * y );
  r = r * ( 1.5f - r * r * y );
  r = r * ( 1.5f - r * r * y );
  return r;
}

KM_INLINE Float Math::Sqrt16( Float x ) {
  return x * InvSqrt16( x );
}

KM_INLINE Float Math::Sqrt( Float x ) {
  return x * InvSqrt( x );
}

KM_INLINE Double Math::Sqrt64( Float x ) {
  return x * InvSqrt64( x );
}

KM_INLINE Float Math::Sin( Float a ) {
  return sinf( a );
}

KM_INLINE Float Math::Sin16( Float a ) {
  Float s;

  if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
    a -= floorf( a / TWO_PI ) * TWO_PI;
  }
#if 1
  if ( a < PI ) {
    if ( a > HALF_PI ) {
      a = PI - a;
    }
  } else {
    if ( a > PI + HALF_PI ) {
      a = a - TWO_PI;
    } else {
      a = PI - a;
    }
  }
#else
  a = PI - a;
  if ( fabs( a ) >= HALF_PI ) {
    a = ( ( a < 0.0f ) ? -PI : PI ) - a;
  }
#endif
  s = a * a;
  return a * ( ( ( ( ( -2.39e-08f * s + 2.7526e-06f ) * s - 1.98409e-04f ) * s + 8.3333315e-03f ) * s - 1.666666664e-01f ) * s + 1.0f );
}

KM_INLINE Double Math::Sin64( Float a ) {
  return sin( a );
}

KM_INLINE Float Math::Cos( Float a ) {
  return cosf( a );
}

KM_INLINE Float Math::Cos16( Float a ) {
  Float s, d;

  if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
    a -= floorf( a / TWO_PI ) * TWO_PI;
  }
#if 1
  if ( a < PI ) {
    if ( a > HALF_PI ) {
      a = PI - a;
      d = -1.0f;
    } else {
      d = 1.0f;
    }
  } else {
    if ( a > PI + HALF_PI ) {
      a = a - TWO_PI;
      d = 1.0f;
    } else {
      a = PI - a;
      d = -1.0f;
    }
  }
#else
  a = PI - a;
  if ( fabs( a ) >= HALF_PI ) {
    a = ( ( a < 0.0f ) ? -PI : PI ) - a;
    d = 1.0f;
  } else {
    d = -1.0f;
  }
#endif
  s = a * a;
  return d * ( ( ( ( ( -2.605e-07f * s + 2.47609e-05f ) * s - 1.3888397e-03f ) * s + 4.16666418e-02f ) * s - 4.999999963e-01f ) * s + 1.0f );
}

KM_INLINE Double Math::Cos64( Float a ) {
  return cos( a );
}

KM_INLINE Void Math::SinCos( Float a, Float &s, Float &c ) {
#ifdef _WIN32
  _asm {
    fld    a
    fsincos
    mov    ecx, c
    mov    edx, s
    fstp  Dword ptr [ecx]
    fstp  Dword ptr [edx]
  }
#else
  s = sinf( a );
  c = cosf( a );
#endif
}

KM_INLINE Void Math::SinCos16( Float a, Float &s, Float &c ) {
  Float t, d;

  if ( ( a < 0.0f ) || ( a >= Math::TWO_PI ) ) {
    a -= floorf( a / Math::TWO_PI ) * Math::TWO_PI;
  }
#if 1
  if ( a < PI ) {
    if ( a > HALF_PI ) {
      a = PI - a;
      d = -1.0f;
    } else {
      d = 1.0f;
    }
  } else {
    if ( a > PI + HALF_PI ) {
      a = a - TWO_PI;
      d = 1.0f;
    } else {
      a = PI - a;
      d = -1.0f;
    }
  }
#else
  a = PI - a;
  if ( fabs( a ) >= HALF_PI ) {
    a = ( ( a < 0.0f ) ? -PI : PI ) - a;
    d = 1.0f;
  } else {
    d = -1.0f;
  }
#endif
  t = a * a;
  s = a * ( ( ( ( ( -2.39e-08f * t + 2.7526e-06f ) * t - 1.98409e-04f ) * t + 8.3333315e-03f ) * t - 1.666666664e-01f ) * t + 1.0f );
  c = d * ( ( ( ( ( -2.605e-07f * t + 2.47609e-05f ) * t - 1.3888397e-03f ) * t + 4.16666418e-02f ) * t - 4.999999963e-01f ) * t + 1.0f );
}

KM_INLINE Void Math::SinCos64( Float a, Double &s, Double &c ) {
#ifdef _WIN32
  _asm {
    fld    a
    fsincos
    mov    ecx, c
    mov    edx, s
    fstp  qword ptr [ecx]
    fstp  qword ptr [edx]
  }
#else
  s = sin( a );
  c = cos( a );
#endif
}

KM_INLINE Float Math::Tan( Float a ) {
  return tanf( a );
}

KM_INLINE Float Math::Tan16( Float a ) {
  Float s;
  Bool reciprocal;

  if ( ( a < 0.0f ) || ( a >= PI ) ) {
    a -= floorf( a / PI ) * PI;
  }
#if 1
  if ( a < HALF_PI ) {
    if ( a > ONEFOURTH_PI ) {
      a = HALF_PI - a;
      reciprocal = true;
    } else {
      reciprocal = false;
    }
  } else {
    if ( a > HALF_PI + ONEFOURTH_PI ) {
      a = a - PI;
      reciprocal = false;
    } else {
      a = HALF_PI - a;
      reciprocal = true;
    }
  }
#else
  a = HALF_PI - a;
  if ( fabs( a ) >= ONEFOURTH_PI ) {
    a = ( ( a < 0.0f ) ? -HALF_PI : HALF_PI ) - a;
    reciprocal = false;
  } else {
    reciprocal = true;
  }
#endif
  s = a * a;
  s = a * ( ( ( ( ( ( 9.5168091e-03f * s + 2.900525e-03f ) * s + 2.45650893e-02f ) * s + 5.33740603e-02f ) * s + 1.333923995e-01f ) * s + 3.333314036e-01f ) * s + 1.0f );
  if ( reciprocal ) {
    return 1.0f / s;
  } else {
    return s;
  }
}

KM_INLINE Double Math::Tan64( Float a ) {
  return tan( a );
}

KM_INLINE Float Math::ASin( Float a ) {
  if ( a <= -1.0f ) {
    return -HALF_PI;
  }
  if ( a >= 1.0f ) {
    return HALF_PI;
  }
  return asinf( a );
}

KM_INLINE Float Math::ASin16( Float a ) {
  if ( FLOATSIGNBITSET( a ) ) {
    if ( a <= -1.0f ) {
      return -HALF_PI;
    }
    a = fabs( a );
    return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a ) - HALF_PI;
  } else {
    if ( a >= 1.0f ) {
      return HALF_PI;
    }
    return HALF_PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
  }
}

KM_INLINE Double Math::ASin64( Float a ) {
  if ( a <= -1.0f ) {
    return -HALF_PI;
  }
  if ( a >= 1.0f ) {
    return HALF_PI;
  }
  return asin( a );
}

KM_INLINE Float Math::ACos( Float a ) {
  if ( a <= -1.0f ) {
    return PI;
  }
  if ( a >= 1.0f ) {
    return 0.0f;
  }
  return acosf( a );
}

KM_INLINE Float Math::ACos16( Float a ) {
  if ( FLOATSIGNBITSET( a ) ) {
    if ( a <= -1.0f ) {
      return PI;
    }
    a = fabs( a );
    return PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
  } else {
    if ( a >= 1.0f ) {
      return 0.0f;
    }
    return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
  }
}

KM_INLINE Double Math::ACos64( Float a ) {
  if ( a <= -1.0f ) {
    return PI;
  }
  if ( a >= 1.0f ) {
    return 0.0f;
  }
  return acos( a );
}

KM_INLINE Float Math::ATan( Float a ) {
  return atanf( a );
}

KM_INLINE Float Math::ATan16( Float a ) {
  Float s;

  if ( fabs( a ) > 1.0f ) {
    a = 1.0f / a;
    s = a * a;
    s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
        * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
    if ( FLOATSIGNBITSET( a ) ) {
      return s - HALF_PI;
    } else {
      return s + HALF_PI;
    }
  } else {
    s = a * a;
    return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
      * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
  }
}

KM_INLINE Double Math::ATan64( Float a ) {
  return atan( a );
}

KM_INLINE Float Math::ATan( Float y, Float x ) {
  return atan2f( y, x );
}

KM_INLINE Float Math::ATan16( Float y, Float x ) {
  Float a, s;

  if ( fabs( y ) > fabs( x ) ) {
    a = x / y;
    s = a * a;
    s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
        * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
    if ( FLOATSIGNBITSET( a ) ) {
      return s - HALF_PI;
    } else {
      return s + HALF_PI;
    }
  } else {
    a = y / x;
    s = a * a;
    return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
      * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
  }
}

KM_INLINE Double Math::ATan64( Float y, Float x ) {
  return atan2( y, x );
}

KM_INLINE Float Math::Pow( Float x, Float y ) {
  return powf( x, y );
}

KM_INLINE Float Math::Pow16( Float x, Float y ) {
  return Exp16( y * Log16( x ) );
}

KM_INLINE Double Math::Pow64( Float x, Float y ) {
  return pow( x, y );
}

KM_INLINE Float Math::Exp( Float f ) {
  return expf( f );
}

KM_INLINE Float Math::Exp16( Float f ) {
  Int i, s, e, m, exponent;
  Float x, x2, y, p, q;

  x = f * 1.44269504088896340f;    // multiply with ( 1 / log( 2 ) )
#if 1
  i = *reinterpret_cast<Int *>(&x);
  s = ( i >> IEEE_FLT_SIGN_BIT );
  e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
  m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
  i = ( ( m >> ( IEEE_FLT_MANTISSA_BITS - e ) ) & ~( e >> 31 ) ) ^ s;
#else
  i = (Int) x;
  if ( x < 0.0f ) {
    i--;
  }
#endif
  exponent = ( i + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS;
  y = *reinterpret_cast<Float *>(&exponent);
  x -= (Float) i;
  if ( x >= 0.5f ) {
    x -= 0.5f;
    y *= 1.4142135623730950488f;  // multiply with sqrt( 2 )
  }
  x2 = x * x;
  p = x * ( 7.2152891511493f + x2 * 0.0576900723731f );
  q = 20.8189237930062f + x2;
  x = y * ( q + p ) / ( q - p );
  return x;
}

KM_INLINE Double Math::Exp64( Float f ) {
  return exp( f );
}

KM_INLINE Float Math::Log( Float f ) {
  return logf( f );
}

KM_INLINE Float Math::Log16( Float f ) {
  Int i, exponent;
  Float y, y2;

  i = *reinterpret_cast<Int *>(&f);
  exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
  i -= ( exponent + 1 ) << IEEE_FLT_MANTISSA_BITS;  // get value in the range [.5, 1>
  y = *reinterpret_cast<Float *>(&i);
  y *= 1.4142135623730950488f;            // multiply with sqrt( 2 )
  y = ( y - 1.0f ) / ( y + 1.0f );
  y2 = y * y;
  y = y * ( 2.000000000046727f + y2 * ( 0.666666635059382f + y2 * ( 0.4000059794795f + y2 * ( 0.28525381498f + y2 * 0.2376245609f ) ) ) );
  y += 0.693147180559945f * ( (Float)exponent + 0.5f );
  return y;
}

KM_INLINE Double Math::Log64( Float f ) {
  return log( f );
}

KM_INLINE Int Math::IPow( Int x, Int y ) {
  Int r; for( r = x; y > 1; y-- ) { r *= x; } return r;
}

KM_INLINE Int Math::ILog2( Float f ) {
  return ( ( (*reinterpret_cast<Int *>(&f)) >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
}

KM_INLINE Int Math::ILog2( Int i ) {
  return ILog2( (Float)i );
}

KM_INLINE Int Math::Abs( Int x ) {
   Int y = x >> 31;
   return ( ( x ^ y ) - y );
}

KM_INLINE Float Math::Fabs( Float f ) {
  Int tmp = *reinterpret_cast<Int *>( &f );
  tmp &= 0x7FFFFFFF;
  return *reinterpret_cast<Float *>( &tmp );
}

KM_INLINE Float Math::Floor( Float f ) {
  return floorf( f );
}

KM_INLINE Float Math::Ceil( Float f ) {
  return ceilf( f );
}

KM_INLINE Float Math::Rint( Float f ) {
  return floorf( f + 0.5f );
}

KM_INLINE Int Math::Ftoi( Float f ) {
  return (Int) f;
}

KM_INLINE Int Math::FtoiFast( Float f ) {
#ifdef _WIN32
  Int i;
  __asm fld    f
  __asm fistp    i    // use default rouding mode (round nearest)
  return i;
#elif 0            // round chop (C/C++ standard)
  Int i, s, e, m, shift;
  i = *reinterpret_cast<Int *>(&f);
  s = i >> IEEE_FLT_SIGN_BIT;
  e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
  m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
  shift = e - IEEE_FLT_MANTISSA_BITS;
  return ( ( ( ( m >> -shift ) | ( m << shift ) ) & ~( e >> 31 ) ) ^ s ) - s;
//#elif defined( __i386__ )
#elif 0
  Int i = 0;
  __asm__ __volatile__ (
              "fld %1\n" \
              "fistp %0\n" \
              : "=m" (i) \
              : "m" (f) );
  return i;
#else
  return (Int) f;
#endif
}

KM_INLINE unsigned long Math::Ftol( Float f ) {
  return (unsigned long) f;
}

KM_INLINE unsigned long Math::FtolFast( Float f ) {
#ifdef _WIN32
  // FIXME: this overflows on 31bits still .. same as FtoiFast
  unsigned long i;
  __asm fld    f
  __asm fistp    i    // use default rouding mode (round nearest)
  return i;
#elif 0            // round chop (C/C++ standard)
  Int i, s, e, m, shift;
  i = *reinterpret_cast<Int *>(&f);
  s = i >> IEEE_FLT_SIGN_BIT;
  e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
  m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
  shift = e - IEEE_FLT_MANTISSA_BITS;
  return ( ( ( ( m >> -shift ) | ( m << shift ) ) & ~( e >> 31 ) ) ^ s ) - s;
//#elif defined( __i386__ )
#elif 0
  // for some reason, on gcc I need to make sure i == 0 before performing a fistp
  Int i = 0;
  __asm__ __volatile__ (
              "fld %1\n" \
              "fistp %0\n" \
              : "=m" (i) \
              : "m" (f) );
  return i;
#else
  return (unsigned long) f;
#endif
}

KM_INLINE signed char Math::ClampChar( Int i ) {
  if ( i < -128 ) {
    return -128;
  }
  if ( i > 127 ) {
    return 127;
  }
  return i;
}

KM_INLINE signed short Math::ClampShort( Int i ) {
  if ( i < -32768 ) {
    return -32768;
  }
  if ( i > 32767 ) {
    return 32767;
  }
  return i;
}

KM_INLINE Int Math::ClampInt( Int min, Int max, Int value ) {
  if ( value < min ) {
    return min;
  }
  if ( value > max ) {
    return max;
  }
  return value;
}

KM_INLINE Float Math::ClampFloat( Float min, Float max, Float value ) {
  if ( value < min ) {
    return min;
  }
  if ( value > max ) {
    return max;
  }
  return value;
}

KM_INLINE Float Math::AngleNormalize360( Float angle ) {
  if ( ( angle >= 360.0f ) || ( angle < 0.0f ) ) {
    angle -= floor( angle / 360.0f ) * 360.0f;
  }
  return angle;
}

KM_INLINE Float Math::AngleNormalize180( Float angle ) {
  angle = AngleNormalize360( angle );
  if ( angle > 180.0f ) {
    angle -= 360.0f;
  }
  return angle;
}

KM_INLINE Float Math::AngleDelta( Float angle1, Float angle2 ) {
  return AngleNormalize180( angle1 - angle2 );
}

template < class T >
KM_INLINE void Math::Swap( T& a, T& b ) {
  const T c = a;
  a = b;
  b = c;
}

template < class T >
KM_INLINE static Bool Math::Between( T _l, T _r, T _x, Float epsilon ) {
  return min( _l, _r ) <= _x + epsilon && _x <= max( _l, _r ) + epsilon;
}

template < class T >
KM_INLINE static Bool Math::Intersect1D( T a, T b, T c, T d, Float epsilon ) {
	if( a > b ) {
    Math::Swap( a, b );
  }
	if( c > d ) {
    Math::Swap( c, d );
  }
  return max( a, c ) <= min( b, d ) + epsilon;
}
