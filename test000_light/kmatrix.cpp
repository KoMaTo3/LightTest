#include "kmatrix.h"

Mat2 mat2_zero( Vec2( 0, 0 ), Vec2( 0, 0 ) );
Mat2 mat2_identity( Vec2( 1, 0 ), Vec2( 0, 1 ) );

/*
============
Mat2::InverseSelf
============
*/
Bool Mat2::InverseSelf( Void ) {
	// 2+4 = 6 multiplications
	//		 1 division
	Double det, invDet, a;

	det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

	if ( Math::Fabs( ( Float ) det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	a = mat[0][0];
	mat[0][0] =  Float( mat[1][1] * invDet );
	mat[0][1] = Float( - mat[0][1] * invDet );
	mat[1][0] = Float( - mat[1][0] * invDet );
	mat[1][1] =  Float( a * invDet );

	return true;
}

/*
============
Mat2::InverseFastSelf
============
*/
Bool Mat2::InverseFastSelf( Void ) {
	// 2+4 = 6 multiplications
	//		 1 division
	Double det, invDet, a;

	det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

	if ( Math::Fabs( Float( det ) ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	a = mat[0][0];
	mat[0][0] =  Float( mat[1][1] * invDet );
	mat[0][1] = Float( - mat[0][1] * invDet );
	mat[1][0] = Float( - mat[1][0] * invDet );
	mat[1][1] =  Float( a * invDet );

	return true;
}

//===============================================================
//
//	Mat3
//
//===============================================================

Mat3 mat3_zero( Vec3( 0, 0, 0 ), Vec3( 0, 0, 0 ), Vec3( 0, 0, 0 ) );
Mat3 mat3_identity( Vec3( 1, 0, 0 ), Vec3( 0, 1, 0 ), Vec3( 0, 0, 1 ) );

/*
============
Mat3::ToAngles
============
*/
Angles Mat3::ToAngles( Void ) const {
	Angles	angles;
	Double		theta;
	Double		cp;
	Float		sp;

	sp = mat[ 0 ][ 2 ];

	// cap off our sin value so that we don't get any NANs
	if ( sp > 1.0f ) {
		sp = 1.0f;
	} else if ( sp < -1.0f ) {
		sp = -1.0f;
	}

	theta = -asin( sp );
	cp = cos( theta );
	if ( cp > 8192.0f * Math::FLT_EPSILON_NUM ) {
		angles.pitch	= Float( RAD2DEG( theta ) );
		angles.yaw		= RAD2DEG( atan2( mat[ 0 ][ 1 ], mat[ 0 ][ 0 ] ) );
		angles.roll		= RAD2DEG( atan2( mat[ 1 ][ 2 ], mat[ 2 ][ 2 ] ) );
	} else {
		angles.pitch	= Float( RAD2DEG( theta ) );
		angles.yaw		= RAD2DEG( -atan2( mat[ 1 ][ 0 ], mat[ 1 ][ 1 ] ) );
		angles.roll		= 0;
	}
	return angles;
}

/*
============
Mat3::Determinant
============
*/
Float Mat3::Determinant( Void ) const {

	Float det2_12_01 = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];
	Float det2_12_02 = mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0];
	Float det2_12_12 = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];

	return mat[0][0] * det2_12_12 - mat[0][1] * det2_12_02 + mat[0][2] * det2_12_01;
}

/*
============
Mat3::InverseSelf
============
*/
Bool Mat3::InverseSelf( Void ) {
	// 18+3+9 = 30 multiplications
	//			 1 division
	Mat3 inverse;
	Double det, invDet;

	inverse[0][0] = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
	inverse[1][0] = mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2];
	inverse[2][0] = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];

	det = mat[0][0] * inverse[0][0] + mat[0][1] * inverse[1][0] + mat[0][2] * inverse[2][0];

	if ( Math::Fabs( Float( det ) ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	inverse[0][1] = mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2];
	inverse[0][2] = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	inverse[1][1] = mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0];
	inverse[1][2] = mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2];
	inverse[2][1] = mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1];
	inverse[2][2] = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

	mat[0][0] = Float( inverse[0][0] * invDet );
	mat[0][1] = Float( inverse[0][1] * invDet );
	mat[0][2] = Float( inverse[0][2] * invDet );

	mat[1][0] = Float( inverse[1][0] * invDet );
	mat[1][1] = Float( inverse[1][1] * invDet );
	mat[1][2] = Float( inverse[1][2] * invDet );

	mat[2][0] = Float( inverse[2][0] * invDet );
	mat[2][1] = Float( inverse[2][1] * invDet );
	mat[2][2] = Float( inverse[2][2] * invDet );

	return true;
}

/*
============
Mat3::InverseFastSelf
============
*/
Bool Mat3::InverseFastSelf( Void ) {
	// 18+3+9 = 30 multiplications
	//			 1 division
	Mat3 inverse;
	Double det, invDet;

	inverse[0][0] = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
	inverse[1][0] = mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2];
	inverse[2][0] = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];

	det = mat[0][0] * inverse[0][0] + mat[0][1] * inverse[1][0] + mat[0][2] * inverse[2][0];

	if ( Math::Fabs( Float( det ) ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	inverse[0][1] = mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2];
	inverse[0][2] = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	inverse[1][1] = mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0];
	inverse[1][2] = mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2];
	inverse[2][1] = mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1];
	inverse[2][2] = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

	mat[0][0] = Float( inverse[0][0] * invDet );
	mat[0][1] = Float( inverse[0][1] * invDet );
	mat[0][2] = Float( inverse[0][2] * invDet );

	mat[1][0] = Float( inverse[1][0] * invDet );
	mat[1][1] = Float( inverse[1][1] * invDet );
	mat[1][2] = Float( inverse[1][2] * invDet );

	mat[2][0] = Float( inverse[2][0] * invDet );
	mat[2][1] = Float( inverse[2][1] * invDet );
	mat[2][2] = Float( inverse[2][2] * invDet );

	return true;
}

/*
============
Mat3::InertiaTranslate
============
*/
Mat3 Mat3::InertiaTranslate( const Float mass, const Vec3 &centerOfMass, const Vec3 &translation ) const {
	Mat3 m;
	Vec3 newCenter;

	newCenter = centerOfMass + translation;

	m[0][0] = mass * ( ( centerOfMass[1] * centerOfMass[1] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[1] * newCenter[1] + newCenter[2] * newCenter[2] ) );
	m[1][1] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[0] * newCenter[0] + newCenter[2] * newCenter[2] ) );
	m[2][2] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[1] * centerOfMass[1] )
				- ( newCenter[0] * newCenter[0] + newCenter[1] * newCenter[1] ) );

	m[0][1] = m[1][0] = mass * ( newCenter[0] * newCenter[1] - centerOfMass[0] * centerOfMass[1] );
	m[1][2] = m[2][1] = mass * ( newCenter[1] * newCenter[2] - centerOfMass[1] * centerOfMass[2] );
	m[0][2] = m[2][0] = mass * ( newCenter[0] * newCenter[2] - centerOfMass[0] * centerOfMass[2] );

	return (*this) + m;
}

/*
============
Mat3::InertiaTranslateSelf
============
*/
Mat3 &Mat3::InertiaTranslateSelf( const Float mass, const Vec3 &centerOfMass, const Vec3 &translation ) {
	Mat3 m;
	Vec3 newCenter;

	newCenter = centerOfMass + translation;

	m[0][0] = mass * ( ( centerOfMass[1] * centerOfMass[1] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[1] * newCenter[1] + newCenter[2] * newCenter[2] ) );
	m[1][1] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[0] * newCenter[0] + newCenter[2] * newCenter[2] ) );
	m[2][2] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[1] * centerOfMass[1] )
				- ( newCenter[0] * newCenter[0] + newCenter[1] * newCenter[1] ) );

	m[0][1] = m[1][0] = mass * ( newCenter[0] * newCenter[1] - centerOfMass[0] * centerOfMass[1] );
	m[1][2] = m[2][1] = mass * ( newCenter[1] * newCenter[2] - centerOfMass[1] * centerOfMass[2] );
	m[0][2] = m[2][0] = mass * ( newCenter[0] * newCenter[2] - centerOfMass[0] * centerOfMass[2] );

	(*this) += m;

	return (*this);
}

/*
============
Mat3::InertiaRotate
============
*/
Mat3 Mat3::InertiaRotate( const Mat3 &rotation ) const {
	// NOTE: the rotation matrix is stored column-major
	return rotation.Transpose() * (*this) * rotation;
}

/*
============
Mat3::InertiaRotateSelf
============
*/
Mat3 &Mat3::InertiaRotateSelf( const Mat3 &rotation ) {
	// NOTE: the rotation matrix is stored column-major
	*this = rotation.Transpose() * (*this) * rotation;
	return *this;
}


//===============================================================
//
//	Mat4
//
//===============================================================

Mat4 mat4_zero( Vec4( 0, 0, 0, 0 ), Vec4( 0, 0, 0, 0 ), Vec4( 0, 0, 0, 0 ), Vec4( 0, 0, 0, 0 ) );
Mat4 mat4_identity( Vec4( 1, 0, 0, 0 ), Vec4( 0, 1, 0, 0 ), Vec4( 0, 0, 1, 0 ), Vec4( 0, 0, 0, 1 ) );

/*
============
Mat4::Transpose
============
*/
Mat4 Mat4::Transpose( Void ) const {
	Mat4	transpose;
	Int		i, j;
   
	for( i = 0; i < 4; i++ ) {
		for( j = 0; j < 4; j++ ) {
			transpose[ i ][ j ] = mat[ j ][ i ];
        }
	}
	return transpose;
}

/*
============
Mat4::TransposeSelf
============
*/
Mat4 &Mat4::TransposeSelf( Void ) {
	Float	temp;
	Int		i, j;
   
	for( i = 0; i < 4; i++ ) {
		for( j = i + 1; j < 4; j++ ) {
			temp = mat[ i ][ j ];
			mat[ i ][ j ] = mat[ j ][ i ];
			mat[ j ][ i ] = temp;
        }
	}
	return *this;
}

/*
============
Mat4::Determinant
============
*/
Float Mat4::Determinant( Void ) const {

	// 2x2 sub-determinants
	Float det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	Float det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
	Float det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
	Float det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	Float det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
	Float det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

	// 3x3 sub-determinants
	Float det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
	Float det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
	Float det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
	Float det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

	return ( - det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3] );
}

/*
============
Mat4::InverseSelf
============
*/
Bool Mat4::InverseSelf( Void ) {
	// 84+4+16 = 104 multiplications
	//			   1 division
	Double det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	Float det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	Float det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
	Float det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
	Float det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	Float det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
	Float det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	Float det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
	Float det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
	Float det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
	Float det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

	det = ( - det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3] );

	if ( Math::Fabs( Float( det ) ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	Float det2_03_01 = mat[0][0] * mat[3][1] - mat[0][1] * mat[3][0];
	Float det2_03_02 = mat[0][0] * mat[3][2] - mat[0][2] * mat[3][0];
	Float det2_03_03 = mat[0][0] * mat[3][3] - mat[0][3] * mat[3][0];
	Float det2_03_12 = mat[0][1] * mat[3][2] - mat[0][2] * mat[3][1];
	Float det2_03_13 = mat[0][1] * mat[3][3] - mat[0][3] * mat[3][1];
	Float det2_03_23 = mat[0][2] * mat[3][3] - mat[0][3] * mat[3][2];

	Float det2_13_01 = mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0];
	Float det2_13_02 = mat[1][0] * mat[3][2] - mat[1][2] * mat[3][0];
	Float det2_13_03 = mat[1][0] * mat[3][3] - mat[1][3] * mat[3][0];
	Float det2_13_12 = mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1];
	Float det2_13_13 = mat[1][1] * mat[3][3] - mat[1][3] * mat[3][1];
	Float det2_13_23 = mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2];

	// remaining 3x3 sub-determinants
	Float det3_203_012 = mat[2][0] * det2_03_12 - mat[2][1] * det2_03_02 + mat[2][2] * det2_03_01;
	Float det3_203_013 = mat[2][0] * det2_03_13 - mat[2][1] * det2_03_03 + mat[2][3] * det2_03_01;
	Float det3_203_023 = mat[2][0] * det2_03_23 - mat[2][2] * det2_03_03 + mat[2][3] * det2_03_02;
	Float det3_203_123 = mat[2][1] * det2_03_23 - mat[2][2] * det2_03_13 + mat[2][3] * det2_03_12;

	Float det3_213_012 = mat[2][0] * det2_13_12 - mat[2][1] * det2_13_02 + mat[2][2] * det2_13_01;
	Float det3_213_013 = mat[2][0] * det2_13_13 - mat[2][1] * det2_13_03 + mat[2][3] * det2_13_01;
	Float det3_213_023 = mat[2][0] * det2_13_23 - mat[2][2] * det2_13_03 + mat[2][3] * det2_13_02;
	Float det3_213_123 = mat[2][1] * det2_13_23 - mat[2][2] * det2_13_13 + mat[2][3] * det2_13_12;

	Float det3_301_012 = mat[3][0] * det2_01_12 - mat[3][1] * det2_01_02 + mat[3][2] * det2_01_01;
	Float det3_301_013 = mat[3][0] * det2_01_13 - mat[3][1] * det2_01_03 + mat[3][3] * det2_01_01;
	Float det3_301_023 = mat[3][0] * det2_01_23 - mat[3][2] * det2_01_03 + mat[3][3] * det2_01_02;
	Float det3_301_123 = mat[3][1] * det2_01_23 - mat[3][2] * det2_01_13 + mat[3][3] * det2_01_12;

	mat[0][0] =	Float( - det3_213_123 * invDet );
	mat[1][0] = Float( + det3_213_023 * invDet );
	mat[2][0] = Float( - det3_213_013 * invDet );
	mat[3][0] = Float( + det3_213_012 * invDet );

	mat[0][1] = Float( + det3_203_123 * invDet );
	mat[1][1] = Float( - det3_203_023 * invDet );
	mat[2][1] = Float( + det3_203_013 * invDet );
	mat[3][1] = Float( - det3_203_012 * invDet );

	mat[0][2] = Float( + det3_301_123 * invDet );
	mat[1][2] = Float( - det3_301_023 * invDet );
	mat[2][2] = Float( + det3_301_013 * invDet );
	mat[3][2] = Float( - det3_301_012 * invDet );

	mat[0][3] = Float( - det3_201_123 * invDet );
	mat[1][3] = Float( + det3_201_023 * invDet );
	mat[2][3] = Float( - det3_201_013 * invDet );
	mat[3][3] = Float( + det3_201_012 * invDet );

	return true;
}

/*
============
Mat4::InverseFastSelf
============
*/
Bool Mat4::InverseFastSelf( Void ) {
	Mat2 r0, r1, r2, r3;
	Float a, det, invDet;
	Float *mat = reinterpret_cast<Float *>(this);

	// r0 = m0.Inverse();
	det = mat[0*4+0] * mat[1*4+1] - mat[0*4+1] * mat[1*4+0];

	if ( Math::Fabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	r0[0][0] =   mat[1*4+1] * invDet;
	r0[0][1] = - mat[0*4+1] * invDet;
	r0[1][0] = - mat[1*4+0] * invDet;
	r0[1][1] =   mat[0*4+0] * invDet;

	// r1 = r0 * m1;
	r1[0][0] = r0[0][0] * mat[0*4+2] + r0[0][1] * mat[1*4+2];
	r1[0][1] = r0[0][0] * mat[0*4+3] + r0[0][1] * mat[1*4+3];
	r1[1][0] = r0[1][0] * mat[0*4+2] + r0[1][1] * mat[1*4+2];
	r1[1][1] = r0[1][0] * mat[0*4+3] + r0[1][1] * mat[1*4+3];

	// r2 = m2 * r1;
	r2[0][0] = mat[2*4+0] * r1[0][0] + mat[2*4+1] * r1[1][0];
	r2[0][1] = mat[2*4+0] * r1[0][1] + mat[2*4+1] * r1[1][1];
	r2[1][0] = mat[3*4+0] * r1[0][0] + mat[3*4+1] * r1[1][0];
	r2[1][1] = mat[3*4+0] * r1[0][1] + mat[3*4+1] * r1[1][1];

	// r3 = r2 - m3;
	r3[0][0] = r2[0][0] - mat[2*4+2];
	r3[0][1] = r2[0][1] - mat[2*4+3];
	r3[1][0] = r2[1][0] - mat[3*4+2];
	r3[1][1] = r2[1][1] - mat[3*4+3];

	// r3.InverseSelf();
	det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];

	if ( Math::Fabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	a = r3[0][0];
	r3[0][0] =   r3[1][1] * invDet;
	r3[0][1] = - r3[0][1] * invDet;
	r3[1][0] = - r3[1][0] * invDet;
	r3[1][1] =   a * invDet;

	// r2 = m2 * r0;
	r2[0][0] = mat[2*4+0] * r0[0][0] + mat[2*4+1] * r0[1][0];
	r2[0][1] = mat[2*4+0] * r0[0][1] + mat[2*4+1] * r0[1][1];
	r2[1][0] = mat[3*4+0] * r0[0][0] + mat[3*4+1] * r0[1][0];
	r2[1][1] = mat[3*4+0] * r0[0][1] + mat[3*4+1] * r0[1][1];

	// m2 = r3 * r2;
	mat[2*4+0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
	mat[2*4+1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
	mat[3*4+0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
	mat[3*4+1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];

	// m0 = r0 - r1 * m2;
	mat[0*4+0] = r0[0][0] - r1[0][0] * mat[2*4+0] - r1[0][1] * mat[3*4+0];
	mat[0*4+1] = r0[0][1] - r1[0][0] * mat[2*4+1] - r1[0][1] * mat[3*4+1];
	mat[1*4+0] = r0[1][0] - r1[1][0] * mat[2*4+0] - r1[1][1] * mat[3*4+0];
	mat[1*4+1] = r0[1][1] - r1[1][0] * mat[2*4+1] - r1[1][1] * mat[3*4+1];

	// m1 = r1 * r3;
	mat[0*4+2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
	mat[0*4+3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
	mat[1*4+2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
	mat[1*4+3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];

	// m3 = -r3;
	mat[2*4+2] = -r3[0][0];
	mat[2*4+3] = -r3[0][1];
	mat[3*4+2] = -r3[1][0];
	mat[3*4+3] = -r3[1][1];

	return true;
}

