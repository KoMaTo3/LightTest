#include "kangles.h"

Angles ang_zero( 0.0f, 0.0f, 0.0f );


/*
=================
Angles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
Angles& Angles::Normalize360( Void ) {
  Int i;

  for ( i = 0; i < 3; i++ ) {
    if ( ( (*this)[i] >= 360.0f ) || ( (*this)[i] < 0.0f ) ) {
      (*this)[i] -= floor( (*this)[i] / 360.0f ) * 360.0f;

      if ( (*this)[i] >= 360.0f ) {
        (*this)[i] -= 360.0f;
      }
      if ( (*this)[i] < 0.0f ) {
        (*this)[i] += 360.0f;
      }
    }
  }

  return *this;
}

/*
=================
Angles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
Angles& Angles::Normalize180( Void ) {
  Normalize360();

  if ( pitch > 180.0f ) {
    pitch -= 360.0f;
  }
  
  if ( yaw > 180.0f ) {
    yaw -= 360.0f;
  }

  if ( roll > 180.0f ) {
    roll -= 360.0f;
  }
  return *this;
}

/*
=================
Angles::ToVectors
=================
*/
Void Angles::ToVectors( Vec3 *forward, Vec3 *right, Vec3 *up ) const {
  Float sr, sp, sy, cr, cp, cy;
  
  Math::SinCos( DEG2RAD( yaw ), sy, cy );
  Math::SinCos( DEG2RAD( pitch ), sp, cp );
  Math::SinCos( DEG2RAD( roll ), sr, cr );

  if ( forward ) {
    forward->Set( cp * cy, cp * sy, -sp );
  }

  if ( right ) {
    right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );
  }

  if ( up ) {
    up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
  }
}

/*
=================
Angles::ToForward
=================
*/
Vec3 Angles::ToForward( Void ) const {
  Float sp, sy, cp, cy;
  
  Math::SinCos( DEG2RAD( yaw ), sy, cy );
  Math::SinCos( DEG2RAD( pitch ), sp, cp );

  return Vec3( cp * cy, cp * sy, -sp );
}

/*
=================
Angles::ToQuat
=================
*/
/*
idQuat Angles::ToQuat( Void ) const {
  Float sx, cx, sy, cy, sz, cz;
  Float sxcy, cxcy, sxsy, cxsy;

  Math::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
  Math::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
  Math::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

  sxcy = sx * cy;
  cxcy = cx * cy;
  sxsy = sx * sy;
  cxsy = cx * sy;

  return idQuat( cxsy*sz - sxcy*cz, -cxsy*cz - sxcy*sz, sxsy*cz - cxcy*sz, cxcy*cz + sxsy*sz );
}
*/

/*
=================
Angles::ToRotation
=================
*/
/*
idRotation Angles::ToRotation( Void ) const {
  Vec3 vec;
  Float angle, w;
  Float sx, cx, sy, cy, sz, cz;
  Float sxcy, cxcy, sxsy, cxsy;

  if ( pitch == 0.0f ) {
    if ( yaw == 0.0f ) {
      return idRotation( vec3_origin, Vec3( -1.0f, 0.0f, 0.0f ), roll );
    }
    if ( roll == 0.0f ) {
      return idRotation( vec3_origin, Vec3( 0.0f, 0.0f, -1.0f ), yaw );
    }
  } else if ( yaw == 0.0f && roll == 0.0f ) {
    return idRotation( vec3_origin, Vec3( 0.0f, -1.0f, 0.0f ), pitch );
  }

  Math::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
  Math::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
  Math::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

  sxcy = sx * cy;
  cxcy = cx * cy;
  sxsy = sx * sy;
  cxsy = cx * sy;

  vec.x =  cxsy * sz - sxcy * cz;
  vec.y = -cxsy * cz - sxcy * sz;
  vec.z =  sxsy * cz - cxcy * sz;
  w =     cxcy * cz + sxsy * sz;
  angle = Math::ACos( w );
  if ( angle == 0.0f ) {
    vec.Set( 0.0f, 0.0f, 1.0f );
  } else {
    //vec *= (1.0f / sin( angle ));
    vec.Normalize();
    vec.FixDegenerateNormal();
    angle *= 2.0f * Math::M_RAD2DEG;
  }
  return idRotation( vec3_origin, vec, angle );
}
*/

/*
=================
Angles::ToMat3
=================
*/
Mat3 Angles::ToMat3( Void ) const {
  Mat3 mat;
  Float sr, sp, sy, cr, cp, cy;

  Math::SinCos( DEG2RAD( yaw ), sy, cy );
  Math::SinCos( DEG2RAD( pitch ), sp, cp );
  Math::SinCos( DEG2RAD( roll ), sr, cr );

  mat[ 0 ].Set( cp * cy, cp * sy, -sp );
  mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
  mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );

  return mat;
}

/*
=================
Angles::ToMat4
=================
*/
Mat4 Angles::ToMat4( Void ) const {
  return ToMat3().ToMat4();
}
