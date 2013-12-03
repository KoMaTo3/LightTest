#include "kmath.h"

const Float Math::PI            = 3.14159265358979323846f;
const Float Math::TWO_PI        = 2.0f * PI;
const Float Math::HALF_PI       = 0.5f * PI;
const Float Math::ONEFOURTH_PI  = 0.25f * PI;
const Float Math::E             = 2.71828182845904523536f;
const Float Math::SQRT_TWO      = 1.41421356237309504880f;
const Float Math::SQRT_THREE    = 1.73205080756887729352f;
const Float Math::SQRT_1OVER2   = 0.70710678118654752440f;
const Float Math::SQRT_1OVER3   = 0.57735026918962576450f;
const Float Math::M_DEG2RAD     = PI / 180.0f;
const Float Math::M_RAD2DEG     = 180.0f / PI;
const Float Math::M_SEC2MS      = 1000.0f;
const Float Math::M_MS2SEC      = 0.001f;
const Float Math::INFINITY      = 1e30f;
const Float Math::FLT_EPSILON_NUM = 1.192092896e-07f;

Bool Math::initialized          = false;
Dword Math::iSqrt[ SQRT_TABLE_SIZE ];


/*
===============
Math::Init
===============
*/
Void Math::Init( Void ) {
  union _flint fi, fo;

  for ( Int i = 0; i < SQRT_TABLE_SIZE; i++ ) {
    fi.i   = ((EXP_BIAS-1) << EXP_POS) | (i << LOOKUP_POS);
    fo.f   = (Float)( 1.0 / sqrt( fi.f ) );
    iSqrt[i] = ((Dword)(((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF))<<SEED_POS;
  }

  iSqrt[SQRT_TABLE_SIZE / 2] = ((Dword)(0xFF))<<(SEED_POS); 

  initialized = true;
}
