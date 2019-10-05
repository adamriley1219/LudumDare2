#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

const Matrix44 Matrix44::IDENTITY;

//--------------------------------------------------------------------------
/**
* Matrix44
*/
Matrix44::Matrix44()
{
	MakeIdentity();
}

//--------------------------------------------------------------------------
/**
* Matrix44
*/
Matrix44::Matrix44( const Vec2& iVector, const Vec2& jVector, const Vec2& tVector )
{
	m_values[ Ix ] = iVector.x;
	m_values[ Iy ] = iVector.y;
	m_values[ Iz ] = 0.0f;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = jVector.x;
	m_values[ Jy ] = jVector.y;
	m_values[ Jz ] = 0.0f;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = 0.0f;
	m_values[ Ky ] = 0.0f;
	m_values[ Kz ] = 1.0f;
	m_values[ Kw ] = 0.0f;

	m_values[ Tx ] = tVector.x;
	m_values[ Ty ] = tVector.y;
	m_values[ Tz ] = 0.0f;
	m_values[ Tw ] = 1.0f;
}

//--------------------------------------------------------------------------
/**
* Matrix44
*/
Matrix44::Matrix44( const Vec3& iVector, const Vec3& jVector, const Vec3& kVector, const Vec3& tVector )
{
	m_values[ Ix ] = iVector.x;
	m_values[ Iy ] = iVector.y;
	m_values[ Iz ] = iVector.z;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = jVector.x;
	m_values[ Jy ] = jVector.y;
	m_values[ Jz ] = jVector.z;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = kVector.x;
	m_values[ Ky ] = kVector.y;
	m_values[ Kz ] = kVector.z;
	m_values[ Kw ] = 0.0f;

	m_values[ Tx ] = tVector.x;
	m_values[ Ty ] = tVector.y;
	m_values[ Tz ] = tVector.z;
	m_values[ Tw ] = 1.0f;
}

//--------------------------------------------------------------------------
/**
* Matrix44
*/
Matrix44::Matrix44( const Vec4& iVector, const Vec4& jVector, const Vec4 &kVector, const Vec4& tVector )
{
	m_values[ Ix ] = iVector.x;
	m_values[ Iy ] = iVector.y;
	m_values[ Iz ] = iVector.z;
	m_values[ Iw ] = iVector.w;

	m_values[ Jx ] = jVector.x;
	m_values[ Jy ] = jVector.y;
	m_values[ Jz ] = jVector.z;
	m_values[ Jw ] = jVector.w;

	m_values[ Kx ] = kVector.x;
	m_values[ Ky ] = kVector.y;
	m_values[ Kz ] = kVector.z;
	m_values[ Kw ] = kVector.w;

	m_values[ Tx ] = tVector.x;
	m_values[ Ty ] = tVector.y;
	m_values[ Tz ] = tVector.z;
	m_values[ Tw ] = tVector.w;
}

//--------------------------------------------------------------------------
/**
* Matrix44
*/
Matrix44::Matrix44( const float sixteenElementArray[16] )
{
	m_values[ Ix ] = sixteenElementArray[0];
	m_values[ Iy ] = sixteenElementArray[1];
	m_values[ Iz ] = sixteenElementArray[2];
	m_values[ Iw ] = sixteenElementArray[3];

	m_values[ Jx ] = sixteenElementArray[4];
	m_values[ Jy ] = sixteenElementArray[5];
	m_values[ Jz ] = sixteenElementArray[6];
	m_values[ Jw ] = sixteenElementArray[7];

	m_values[ Kx ] = sixteenElementArray[8];
	m_values[ Ky ] = sixteenElementArray[9];
	m_values[ Kz ] = sixteenElementArray[10];
	m_values[ Kw ] = sixteenElementArray[11];

	m_values[ Tx ] = sixteenElementArray[12];
	m_values[ Ty ] = sixteenElementArray[13];
	m_values[ Tz ] = sixteenElementArray[14];
	m_values[ Tw ] = sixteenElementArray[15];
}

//--------------------------------------------------------------------------
/**
* ~Matrix44
*/
Matrix44::~Matrix44()
{

}

//--------------------------------------------------------------------------
/**
* MakeXRotationDegrees
*/
Matrix44 Matrix44::MakeXRotationDegrees( float degreesAboutZ )
{
	Matrix44 translationMatrix;
	float cos = CosDegrees( degreesAboutZ );
	float sin = SinDegrees( degreesAboutZ );

	translationMatrix.m_values[ Jy ] = cos;
	translationMatrix.m_values[ Jz ] = sin;

	translationMatrix.m_values[ Ky ] = -sin;
	translationMatrix.m_values[ Kz ] = cos;

	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeYRotationDegrees
*/
Matrix44 Matrix44::MakeYRotationDegrees( float degreesAboutZ )
{
	Matrix44 translationMatrix;
	float cos = CosDegrees( degreesAboutZ );
	float sin = SinDegrees( degreesAboutZ );

	translationMatrix.m_values[ Ix ] = cos;
	translationMatrix.m_values[ Iz ] = -sin;

	translationMatrix.m_values[ Kx ] = sin;
	translationMatrix.m_values[ Kz ] = cos;

	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeZRotationDegrees
*/
Matrix44 Matrix44::MakeZRotationDegrees( float degreesAboutZ )
{
	Matrix44 translationMatrix;
	float cos = CosDegrees( degreesAboutZ );
	float sin = SinDegrees( degreesAboutZ );

	translationMatrix.m_values[ Ix ] = cos;
	translationMatrix.m_values[ Iy ] = sin;

	translationMatrix.m_values[ Jx ] = -sin;
	translationMatrix.m_values[ Jy ] = cos;

	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeRotationAboutAxis
* https://en.wikipedia.org/wiki/Rotation_matrix
*/
Matrix44 Matrix44::MakeRotationAboutAxis( const Vec3& axis, float angleDegrees )
{
	Vec3 nAxis = axis.GetNormalized();
	Matrix44 mat;
	float cos = CosDegrees( angleDegrees );
	float sin = SinDegrees( angleDegrees );
	
	mat.m_values[Ix] = cos + nAxis.x * nAxis.x * ( 1.0f - cos );
	mat.m_values[Iy] = nAxis.x * nAxis.y * ( 1.0f - cos ) + nAxis.z * sin;
	mat.m_values[Iz] = nAxis.z * nAxis.x * ( 1.0f - cos ) - nAxis.y * sin;

	mat.m_values[Jx] = nAxis.x * nAxis.y * ( 1.0f - cos ) - nAxis.z * sin;
	mat.m_values[Jy] = cos + nAxis.y * nAxis.y * ( 1.0f - cos );
	mat.m_values[Jz] = nAxis.z * nAxis.y * ( 1.0f - cos ) + nAxis.x * sin;

	mat.m_values[Kx] = nAxis.x * nAxis.z * ( 1.0f - cos ) + nAxis.y * sin;
	mat.m_values[Ky] = nAxis.y * nAxis.z * ( 1.0f - cos ) - nAxis.x * sin;
	mat.m_values[Kz] = cos + nAxis.z * nAxis.z * ( 1.0f - cos );


	return mat;
}

//--------------------------------------------------------------------------
/**
* MakeRotationFromDisplacementWithX
*/
Matrix44 Matrix44::MakeRotationFromDisplacementWithX( const Vec3& displacement )
{
	Vec3 right = displacement.GetNormalized();

	Matrix44 transformMat = right != Vec3( 1.0f, 0.0f, 0.0f )
		? Matrix44::MakeRotationAboutAxis( Cross( Vec3( 1.0f, 0.0f, 0.0f ), right ), ArccosDegrees( DotProduct( right, Vec3( 1.0f, 0.0f, 0.0f ) ) ) ) 
		: Matrix44::IDENTITY;
	return transformMat;

}

//--------------------------------------------------------------------------
/**
* MakeRotationFromDisplacementWithY
*/
Matrix44 Matrix44::MakeRotationFromDisplacementWithY( const Vec3& displacement )
{
	Vec3 up = displacement.GetNormalized();

	Matrix44 transformMat = up != Vec3( 0.0f, 1.0f, 0.0f )
		? Matrix44::MakeRotationAboutAxis( Cross( Vec3( 0.0f, 1.0f, 0.0f ), up ), ArccosDegrees( DotProduct( up, Vec3( 0.0f, 1.0f, 0.0f ) ) ) ) 
		: Matrix44::IDENTITY;
	return transformMat;
}

//--------------------------------------------------------------------------
/**
* MakeRotationFromDisplacementWithZ
*/
Matrix44 Matrix44::MakeRotationFromDisplacementWithZ( const Vec3& displacement )
{
	Vec3 forward = displacement.GetNormalized();

	Matrix44 transformMat = forward != Vec3( 0.0f, 0.0f, 1.0f )
		? Matrix44::MakeRotationAboutAxis( Cross( Vec3( 0.0f, 0.0f, 1.0f ), forward ), ArccosDegrees( DotProduct( forward, Vec3( 0.0f, 0.0f, 1.0f ) ) ) ) 
		: Matrix44::IDENTITY;
	return transformMat;
}

//--------------------------------------------------------------------------
/**
* MakeTranslation2D
*/
Matrix44 Matrix44::MakeTranslation2D( const Vec2& translationXY )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[ Tx ] = translationXY.x;
	translationMatrix.m_values[ Ty ] = translationXY.y;
	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeTranslation3D
*/
Matrix44 Matrix44::MakeTranslation3D( const Vec3& translationXY )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[ Tx ] = translationXY.x;
	translationMatrix.m_values[ Ty ] = translationXY.y;
	translationMatrix.m_values[ Tz ] = translationXY.z;
	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeUniformScale2D
*/
Matrix44 Matrix44::MakeUniformScale2D( float uniformScaleXY )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[ Ix ] = uniformScaleXY;
	translationMatrix.m_values[ Jy ] = uniformScaleXY;
	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeUniformScale3D
*/
Matrix44 Matrix44::MakeUniformScale3D( float uniformScaleXY )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[ Ix ] = uniformScaleXY;
	translationMatrix.m_values[ Jy ] = uniformScaleXY;
	translationMatrix.m_values[ Kz ] = uniformScaleXY;
	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeScale2D
*/
Matrix44 Matrix44::MakeScale2D( const Vec2& uniformScaleXY )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[ Ix ] = uniformScaleXY.x;
	translationMatrix.m_values[ Jy ] = uniformScaleXY.y;
	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeScale3D
*/
Matrix44 Matrix44::MakeScale3D( const Vec3& uniformScaleXY )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[ Ix ] = uniformScaleXY.x;
	translationMatrix.m_values[ Jy ] = uniformScaleXY.y;
	translationMatrix.m_values[ Kz ] = uniformScaleXY.z;
	return translationMatrix;
}

//--------------------------------------------------------------------------
/**
* MakeProjectionPerspective
*/
Matrix44 Matrix44::MakeProjectionPerspective( float fovDegrees, float aspect, float nearZ, float farZ )
{
	Matrix44 mat;
	float rad = ConvertDegreesToRadians( fovDegrees );
	float size = 1 / tanf( rad * .5f );
	float zDif = 1.0f / ( farZ - nearZ );
	mat.m_values[Matrix44::Ix] = size / aspect;
	mat.m_values[Matrix44::Jy] = size;
	mat.m_values[Matrix44::Kz] = farZ * zDif;
	mat.m_values[Matrix44::Kw] = 1.0f;
	mat.m_values[Matrix44::Tz] = -nearZ * farZ * zDif;
	mat.m_values[Matrix44::Tw] = 0.0f;
	return mat;
}

//--------------------------------------------------------------------------
/**
* MakeProjectionOrthographic
*/
Matrix44 Matrix44::MakeProjectionOrthographic( const Vec2& min, const Vec2& max, float nearZ, float farZ )
{
	float run = max.x - min.x;
	float rise = max.y - min.y;
	float sx = 1.0f / run;
	float sy = 1.0f / rise;
	float sz = 1.0f / ( farZ - nearZ );

	Matrix44 mat;
	mat.m_values[Matrix44::Ix] = 2.0f * sx;
	mat.m_values[Matrix44::Jy] = 2.0f * sy;
	mat.m_values[Matrix44::Kz] = sz;

	mat.m_values[Matrix44::Tx] = -( max.x + min.x ) * sx;
	mat.m_values[Matrix44::Ty] = -( max.y + min.y ) * sy;
	mat.m_values[Matrix44::Tz] = -nearZ * sz;

	return mat;
}

//--------------------------------------------------------------------------
/**
* FromEuler
*/
Matrix44 Matrix44::FromEuler( Vec3 euler, eRotationOrder rot )
{
	Matrix44 rotx = MakeXRotationDegrees( euler.x ); 
	Matrix44 roty = MakeYRotationDegrees( euler.y ); 
	Matrix44 rotz = MakeZRotationDegrees( euler.z ); 

	// for games, we usually go around
	// forward, then right, then up
	Matrix44 ret; 
	if (rot == ROTATION_ORDER_ZXY) 
	{
 		ret = rotz;  // same as rotz.transform(ret) since ret is the identity
 		ret = rotx.Concatenate(ret); 
 		ret = roty.Concatenate(ret); 
	}
	else if(rot == ROTATION_ORDER_XYZ)
	{
		ret = rotx;
		ret = roty.Concatenate(ret);
		ret = rotz.Concatenate(ret);
	} 
	else 
	{
		// other cases
		ASSERT_RECOVERABLE(false, "Not Implemented Euler case."); // implement unknown case
	}

	return ret; 
}

//--------------------------------------------------------------------------
/**
* LookAt
*/
Matrix44 Matrix44::LookAt( const Vec3& start, const Vec3& end, const Vec3& upRef )
{
	Vec3 curUp = upRef.GetNormalized();
	Vec3 forward = (end - start).GetNormalized();

	if( curUp == forward )
	{
		curUp = Vec3( 1.0f, 0.0f, 0.0f );
	}
	if( curUp == forward * -1.0f )
	{
		curUp = Vec3( -1.0f, 0.0f, 0.0f );
	}

	Vec3 right = Cross( upRef, forward ).GetNormalized();
	Vec3 up = Cross( forward, right );

	Matrix44 mat;
	mat.SetI( right );
	mat.SetJ( up );
	mat.SetK( forward );
	mat.SetT( start );
	return mat;
}

//--------------------------------------------------------------------------
/**
* GetRow
*/
Vec4 Matrix44::GetRow( int rowIndex ) const
{
	return Vec4( m_values[Ix * rowIndex], m_values[Jx * rowIndex], m_values[Kx * rowIndex], m_values[Tx * rowIndex] );
}

//--------------------------------------------------------------------------
/**
* GetCol
*/
Vec4 Matrix44::GetCol( int colIndex ) const
{
	return m_cols[colIndex];
}

//--------------------------------------------------------------------------
/**
* SetI
*/
void Matrix44::SetI( const Vec4& values )
{
	m_cols[0] = values;
}

//--------------------------------------------------------------------------
/**
* SetI
*/
void Matrix44::SetI( const Vec3& values )
{
	m_cols[0].x = values.x;
	m_cols[0].y = values.y;
	m_cols[0].z = values.z;
}

//--------------------------------------------------------------------------
/**
* SetJ
*/
void Matrix44::SetJ( const Vec4& values )
{
	m_cols[1] = values;
}

//--------------------------------------------------------------------------
/**
* SetJ
*/
void Matrix44::SetJ( const Vec3& values )
{
	m_cols[1].x = values.x;
	m_cols[1].y = values.y;
	m_cols[1].z = values.z;
}

//--------------------------------------------------------------------------
/**
* SetK
*/
void Matrix44::SetK( const Vec4& values )
{
	m_cols[2] = values;
}

//--------------------------------------------------------------------------
/**
* SetK
*/
void Matrix44::SetK( const Vec3& values )
{
	m_cols[2].x = values.x;
	m_cols[2].y = values.y;
	m_cols[2].z = values.z;
}

//--------------------------------------------------------------------------
/**
* SetT
*/
void Matrix44::SetT( const Vec4& values )
{
	m_cols[3] = values;
}

//--------------------------------------------------------------------------
/**
* SetT
*/
void Matrix44::SetT( const Vec3& values )
{
	m_cols[3].x = values.x;
	m_cols[3].y = values.y;
	m_cols[3].z = values.z;
}

//--------------------------------------------------------------------------
/**
* SetCol
*/
void Matrix44::SetCol( const Vec4& values, unsigned int colIndex )
{
	m_cols[colIndex] = values;
}

//--------------------------------------------------------------------------
/**
* SetCol
*/
void Matrix44::SetCol( const Vec3& values, unsigned int colIndex )
{
	m_cols[colIndex].x = values.x;
	m_cols[colIndex].y = values.y;
	m_cols[colIndex].z = values.z;
}

//--------------------------------------------------------------------------
/**
* Set
*/
void Matrix44::Set( const Matrix44& mat )
{
	SetI( mat.GetI() );
	SetJ( mat.GetJ() );
	SetK( mat.GetK() );
	SetT( mat.GetT() );
}

//--------------------------------------------------------------------------
/**
* TransformVec2D
*/
const Vec2 Matrix44::TransformVec2D( const Vec2& vectorQuantity ) const
{
	Vec2 result;

	result.x = ( vectorQuantity.x * m_values[ Ix ] ) + ( vectorQuantity.y * m_values[ Jx ] );
	result.y = ( vectorQuantity.x * m_values[ Iy ] ) + ( vectorQuantity.y * m_values[ Jy ] );

	return result;
}

//--------------------------------------------------------------------------
/**
* TransformPosition2D
*/
const Vec2 Matrix44::TransformPosition2D( const Vec2& position ) const
{
	Vec2 result;

	result.x = ( position.x * m_values[ Ix ] ) + ( position.y * m_values[ Jx ] ) + m_values[ Tx ];
	result.y = ( position.x * m_values[ Iy ] ) + ( position.y * m_values[ Jy ] ) + m_values[ Ty ];

	return result;
}

//--------------------------------------------------------------------------
/**
* TransformVec3D
*/
const Vec3 Matrix44::TransformVec3D( const Vec3& vectorQuantity ) const
{
	Vec3 result;

	result.x = ( vectorQuantity.x * m_values[ Ix ] ) + ( vectorQuantity.y * m_values[ Jx ] ) + ( vectorQuantity.z * m_values[ Kx ] );
	result.y = ( vectorQuantity.x * m_values[ Iy ] ) + ( vectorQuantity.y * m_values[ Jy ] ) + ( vectorQuantity.z * m_values[ Ky ] );
	result.z = ( vectorQuantity.x * m_values[ Iz ] ) + ( vectorQuantity.y * m_values[ Jz ] ) + ( vectorQuantity.z * m_values[ Kz ] );

	return result;
}

//--------------------------------------------------------------------------
/**
* TransformPosition3D
*/
const Vec3 Matrix44::TransformPosition3D( const Vec3& position ) const
{
	Vec3 result;

	result.x = ( position.x * m_values[ Ix ] ) + ( position.y * m_values[ Jx ] ) + ( position.z * m_values[ Kx ] ) + m_values[ Tx ];
	result.y = ( position.x * m_values[ Iy ] ) + ( position.y * m_values[ Jy ] ) + ( position.z * m_values[ Ky ] ) + m_values[ Ty ];
	result.z = ( position.x * m_values[ Iz ] ) + ( position.y * m_values[ Jz ] ) + ( position.z * m_values[ Kz ] ) + m_values[ Tz ];

	return result;
}

//--------------------------------------------------------------------------
/**
* TransformHomogeneous3D
*/
const	Vec4 Matrix44::TransformHomoPoint3D( const Vec4& homogeneousPoint ) const
{
	Vec4 result;

	result.x = ( homogeneousPoint.x * m_values[ Ix ] ) + ( homogeneousPoint.y * m_values[ Jx ] ) + ( homogeneousPoint.z * m_values[ Kx ] ) + ( homogeneousPoint.w * m_values[ Tx ] );
	result.y = ( homogeneousPoint.x * m_values[ Iy ] ) + ( homogeneousPoint.y * m_values[ Jy ] ) + ( homogeneousPoint.z * m_values[ Ky ] ) + ( homogeneousPoint.w * m_values[ Ty ] );
	result.z = ( homogeneousPoint.x * m_values[ Iz ] ) + ( homogeneousPoint.y * m_values[ Jz ] ) + ( homogeneousPoint.z * m_values[ Kz ] ) + ( homogeneousPoint.w * m_values[ Tz ] );
	result.w = ( homogeneousPoint.x * m_values[ Iw ] ) + ( homogeneousPoint.y * m_values[ Jw ] ) + ( homogeneousPoint.z * m_values[ Kw ] ) + ( homogeneousPoint.w * m_values[ Tw ] );

	return result;
}

//--------------------------------------------------------------------------
/**
* Concatenate
*/
const Matrix44 Matrix44::Concatenate( const Matrix44& other ) const
{
	return *this * other;
}

//--------------------------------------------------------------------------
/**
* MakeIdentity
*/
void Matrix44::MakeIdentity()
{
	m_values[ Ix ] = 1.0f;
	m_values[ Iy ] = 0.0f;
	m_values[ Iz ] = 0.0f;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = 0.0f;
	m_values[ Jy ] = 1.0f;
	m_values[ Jz ] = 0.0f;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = 0.0f;
	m_values[ Ky ] = 0.0f;
	m_values[ Kz ] = 1.0f;
	m_values[ Kw ] = 0.0f;

	m_values[ Tx ] = 0.0f;
	m_values[ Ty ] = 0.0f;
	m_values[ Tz ] = 0.0f;
	m_values[ Tw ] = 1.0f;
}

//--------------------------------------------------------------------------
/**
* Transpose
*/
void Matrix44::Transpose()
{
	Vec4 colI = GetI();
	Vec4 colJ = GetJ();
	Vec4 colK = GetK();
	Vec4 colT = GetT();

//	m_values[ Ix ] = colI.x;
	m_values[ Iy ] = colJ.x;
	m_values[ Iz ] = colK.x;
	m_values[ Iw ] = colT.x;

	m_values[ Jx ] = colI.y;
//	m_values[ Jy ] = colJ.y;
	m_values[ Jz ] = colK.y;
	m_values[ Jw ] = colT.y;

	m_values[ Kx ] = colI.z;
	m_values[ Ky ] = colJ.z;
//	m_values[ Kz ] = colK.z;
	m_values[ Kw ] = colT.z;

	m_values[ Tx ] = colI.w;
	m_values[ Ty ] = colJ.w;
	m_values[ Tz ] = colK.w;
//	m_values[ Tw ] = colT.w;
}

//--------------------------------------------------------------------------
/**
* InvertOrthonormal
*/
void Matrix44::InvertOrthonormal()
{
	// Most transforms are used in games will end up being orthonormal or othogonal
	// "Orthogonoal" - all basis vectors are orthogonoal to each other (ie, Dot(i,j) = 0, Dot(j,k) = 0, and Dot(i,k) = 0)
	// "Orthonormal" - orthogonal where each basis vector is unit length; 

	// An orthonormal matrix represents a Rotation & Translation; 
	// R = Rotation;
	// OBJ_TYPE = Translation
	// this = R * OBJ_TYPE; 

	// So, I can invert by computing
	// invert(OBJ_TYPE) * invert(R)

	// mat44 m = R * OBJ_TYPE; 
	// then inverse(m) = inverse(OBJ_TYPE) * inverse(R)
	// inverse(R) = Transpose(R) (since it is orthonormal without translation)
	// inverse(OBJ_TYPE) = -OBJ_TYPE (just negate the translation part); 

	Vec4 translation = GetT();
	Vec3 trans = Vec3( translation );
	SetT( Vec3::ZERO );
	Transpose();
	trans *= -1.0f;
	Vec3 newTranslation = TransformPosition3D( trans );
	SetT( newTranslation );
}

//--------------------------------------------------------------------------
/**
* Invert
*/
void Matrix44::Invert()
{
	double inv[16];
	double det;
	double m[16];
	int i;

	for (i = 0; i < 16; ++i) {
		m[i] = (double) m_values[i];
	}

	inv[0] = m[5]  * m[10] * m[15] - 
		m[5]  * m[11] * m[14] - 
		m[9]  * m[6]  * m[15] + 
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] - 
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
		m[4]  * m[11] * m[14] + 
		m[8]  * m[6]  * m[15] - 
		m[8]  * m[7]  * m[14] - 
		m[12] * m[6]  * m[11] + 
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
		m[4]  * m[11] * m[13] - 
		m[8]  * m[5] * m[15] + 
		m[8]  * m[7] * m[13] + 
		m[12] * m[5] * m[11] - 
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] - 
		m[8]  * m[6] * m[13] - 
		m[12] * m[5] * m[10] + 
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
		m[1]  * m[11] * m[14] + 
		m[9]  * m[2] * m[15] - 
		m[9]  * m[3] * m[14] - 
		m[13] * m[2] * m[11] + 
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
		m[0]  * m[11] * m[14] - 
		m[8]  * m[2] * m[15] + 
		m[8]  * m[3] * m[14] + 
		m[12] * m[2] * m[11] - 
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
		m[0]  * m[11] * m[13] + 
		m[8]  * m[1] * m[15] - 
		m[8]  * m[3] * m[13] - 
		m[12] * m[1] * m[11] + 
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
		m[0]  * m[10] * m[13] - 
		m[8]  * m[1] * m[14] + 
		m[8]  * m[2] * m[13] + 
		m[12] * m[1] * m[10] - 
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
		m[1]  * m[7] * m[14] - 
		m[5]  * m[2] * m[15] + 
		m[5]  * m[3] * m[14] + 
		m[13] * m[2] * m[7] - 
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
		m[0]  * m[7] * m[14] + 
		m[4]  * m[2] * m[15] - 
		m[4]  * m[3] * m[14] - 
		m[12] * m[2] * m[7] + 
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
		m[0]  * m[7] * m[13] - 
		m[4]  * m[1] * m[15] + 
		m[4]  * m[3] * m[13] + 
		m[12] * m[1] * m[7] - 
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
		m[0]  * m[6] * m[13] + 
		m[4]  * m[1] * m[14] - 
		m[4]  * m[2] * m[13] - 
		m[12] * m[1] * m[6] + 
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
		m[1] * m[7] * m[10] + 
		m[5] * m[2] * m[11] - 
		m[5] * m[3] * m[10] - 
		m[9] * m[2] * m[7] + 
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
		m[0] * m[7] * m[10] - 
		m[4] * m[2] * m[11] + 
		m[4] * m[3] * m[10] + 
		m[8] * m[2] * m[7] - 
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
		m[0] * m[7] * m[9] + 
		m[4] * m[1] * m[11] - 
		m[4] * m[3] * m[9] - 
		m[8] * m[1] * m[7] + 
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
		m[0] * m[6] * m[9] - 
		m[4] * m[1] * m[10] + 
		m[4] * m[2] * m[9] + 
		m[8] * m[1] * m[6] - 
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	for (i = 0; i < 16; i++) {
		m_values[i] = (float)(inv[i] * det);
	}
}

//--------------------------------------------------------------------------
/**
* GetInverted
*/
Matrix44 Matrix44::GetInverted() const
{
	Matrix44 temp = *this;
	temp.Invert();
	return temp;
}

//--------------------------------------------------------------------------
/**
* RotateAroundX
*/
void Matrix44::RotateAroundX( float degrees )
{
	Matrix44 mat = MakeXRotationDegrees(degrees);
	Set( Concatenate( mat ) );
}

//--------------------------------------------------------------------------
/**
* RotateAroundY
*/
void Matrix44::RotateAroundY( float degrees )
{
	Matrix44 mat = MakeYRotationDegrees(degrees);
	Set( Concatenate( mat ) );
}

//--------------------------------------------------------------------------
/**
* RotateAroundZ
*/
void Matrix44::RotateAroundZ( float degrees )
{
	Matrix44 mat = MakeZRotationDegrees(degrees);
	Set( Concatenate( mat ) );
}

//--------------------------------------------------------------------------
/**
* Translate
*/
void Matrix44::Translate( Vec3 offset )
{
	m_values[Tx] += offset.x;
	m_values[Ty] += offset.y;
	m_values[Tz] += offset.z;
}

//--------------------------------------------------------------------------
/**
* operator*
*/
const Matrix44 Matrix44::operator*( Matrix44 concate ) const
{
	Matrix44 mat;

	mat.m_values[ Ix ] 
		=  m_values[Ix] * concate.m_values[Ix] 
		+  m_values[Jx] * concate.m_values[Iy] 
		+  m_values[Kx] * concate.m_values[Iz] 
		+  m_values[Tx] * concate.m_values[Iw];
	mat.m_values[ Iy ] 
		=  m_values[Iy] * concate.m_values[Ix] 
		+  m_values[Jy] * concate.m_values[Iy] 
		+  m_values[Ky] * concate.m_values[Iz] 
		+  m_values[Ty] * concate.m_values[Iw];
	mat.m_values[ Iz ] 
		=  m_values[Iz] * concate.m_values[Ix] 
		+  m_values[Jz] * concate.m_values[Iy] 
		+  m_values[Kz] * concate.m_values[Iz] 
		+  m_values[Tz] * concate.m_values[Iw];
	mat.m_values[ Iw ] 
		=  m_values[Iw] * concate.m_values[Ix] 
		+  m_values[Jw] * concate.m_values[Iy] 
		+  m_values[Kw] * concate.m_values[Iz] 
		+  m_values[Tw] * concate.m_values[Iw];

	mat.m_values[ Jx ] 
		=  m_values[Ix] * concate.m_values[Jx] 
		+  m_values[Jx] * concate.m_values[Jy] 
		+  m_values[Kx] * concate.m_values[Jz] 
		+  m_values[Tx] * concate.m_values[Jw];
	mat.m_values[ Jy ] 
		=  m_values[Iy] * concate.m_values[Jx] 
		+  m_values[Jy] * concate.m_values[Jy] 
		+  m_values[Ky] * concate.m_values[Jz] 
		+  m_values[Ty] * concate.m_values[Jw];
	mat.m_values[ Jz ] 
		=  m_values[Iz] * concate.m_values[Jx] 
		+  m_values[Jz] * concate.m_values[Jy] 
		+  m_values[Kz] * concate.m_values[Jz] 
		+  m_values[Tz] * concate.m_values[Jw];
	mat.m_values[ Jw ] 
		=  m_values[Iw] * concate.m_values[Jx] 
		+  m_values[Jw] * concate.m_values[Jy] 
		+  m_values[Kw] * concate.m_values[Jz] 
		+  m_values[Tw] * concate.m_values[Jw];

	mat.m_values[ Kx ] 
		=  m_values[Ix] * concate.m_values[Kx] 
		+  m_values[Jx] * concate.m_values[Ky] 
		+  m_values[Kx] * concate.m_values[Kz] 
		+  m_values[Tx] * concate.m_values[Kw];
	mat.m_values[ Ky ]
		=  m_values[Iy] * concate.m_values[Kx] 
		+  m_values[Jy] * concate.m_values[Ky] 
		+  m_values[Ky] * concate.m_values[Kz] 
		+  m_values[Ty] * concate.m_values[Kw];
	mat.m_values[ Kz ] 
		=  m_values[Iz] * concate.m_values[Kx] 
		+  m_values[Jz] * concate.m_values[Ky] 
		+  m_values[Kz] * concate.m_values[Kz] 
		+  m_values[Tz] * concate.m_values[Kw];
	mat.m_values[ Kw ] 
		=  m_values[Iw] * concate.m_values[Kx] 
		+  m_values[Jw] * concate.m_values[Ky] 
		+  m_values[Kw] * concate.m_values[Kz] 
		+  m_values[Tw] * concate.m_values[Kw];

	mat.m_values[ Tx ] 
		=  m_values[Ix] * concate.m_values[Tx] 
		+  m_values[Jx] * concate.m_values[Ty] 
		+  m_values[Kx] * concate.m_values[Tz] 
		+  m_values[Tx] * concate.m_values[Tw];
	mat.m_values[ Ty ] 
		=  m_values[Iy] * concate.m_values[Tx] 
		+  m_values[Jy] * concate.m_values[Ty] 
		+  m_values[Ky] * concate.m_values[Tz] 
		+  m_values[Ty] * concate.m_values[Tw];
	mat.m_values[ Tz ] 
		=  m_values[Iz] * concate.m_values[Tx] 
		+  m_values[Jz] * concate.m_values[Ty] 
		+  m_values[Kz] * concate.m_values[Tz] 
		+  m_values[Tz] * concate.m_values[Tw];
	mat.m_values[ Tw ] 
		=  m_values[Iw] * concate.m_values[Tx] 
		+  m_values[Jw] * concate.m_values[Ty] 
		+  m_values[Kw] * concate.m_values[Tz] 
		+  m_values[Tw] * concate.m_values[Tw];
	return mat;
}

//--------------------------------------------------------------------------
/**
* operator*
*/
const Matrix44 Matrix44::operator*( float scale ) const
{
	Matrix44 mat = *this;
	mat.m_values[ Ix ] *= scale;
	mat.m_values[ Iy ] *= scale;
	mat.m_values[ Iz ] *= scale;
	mat.m_values[ Iw ] *= scale;

	mat.m_values[ Jx ] *= scale;
	mat.m_values[ Jy ] *= scale;
	mat.m_values[ Jz ] *= scale;
	mat.m_values[ Jw ] *= scale;

	mat.m_values[ Kx ] *= scale;
	mat.m_values[ Ky ] *= scale;
	mat.m_values[ Kz ] *= scale;
	mat.m_values[ Kw ] *= scale;

	mat.m_values[ Tx ] *= scale;
	mat.m_values[ Ty ] *= scale;
	mat.m_values[ Tz ] *= scale;
	mat.m_values[ Tw ] *= scale;

	return mat;
}

//--------------------------------------------------------------------------
/**
* operator*
*/
const Vec4 Matrix44::operator*( const Vec4& vector ) const
{
	return TransformHomoPoint3D( vector );
}

//--------------------------------------------------------------------------
/**
* TransformPoints
*/
void TransformPoints( uint count, Vec3* points, const Matrix44& matrix )
{
	for( uint idx = 0; idx < count; ++idx )
	{
		Vec4 res = matrix * Vec4( points[idx], 1.0f );
		points[idx] = Vec3( res / res.w );
	}
}


//--------------------------------------------------------------------------
/**
* GetTransfromFromString
* Assumes that there are 3 strings within string that will represent teh state of the transform.
* e.g. -z x -y
*/
Matrix44 GetTransfromFromString(const std::string& string)
{
	Matrix44 mat;
	Vec4 i = mat.GetI();
	Vec4 j = mat.GetJ();
	Vec4 k = mat.GetK();

	std::vector<std::string> splits = SplitStringOnDelitmiter(string.c_str(), " ");

	for (int idx = 0; idx < 3; ++idx)
	{
		if (splits[idx] == "x")
		{
			mat.SetCol(i, idx);
		}
		if (splits[idx] == "y")
		{
			mat.SetCol(j, idx);
		}
		if (splits[idx] == "z")
		{
			mat.SetCol(k, idx);
		}
		if (splits[idx] == "-x")
		{
			mat.SetCol(i * -1.0f, idx);
		}
		if (splits[idx] == "-y")
		{
			mat.SetCol(j * -1.0f, idx);
		}
		if (splits[idx] == "-z")
		{
			mat.SetCol(k * -1.0f, idx);
		}
	}

	return mat;
}