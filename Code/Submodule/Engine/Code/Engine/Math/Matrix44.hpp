#pragma once
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Strings/StringUtils.hpp"

struct Vec2;
struct Vec3;
struct Vec4;

typedef unsigned int uint;

enum eRotationOrder
{
	ROTATION_ORDER_XYZ,     // Unreal/Squirrel Defualt
	ROTATION_ORDER_ZXY,     // Unity/Forseth Defualt

	ROTATION_ORDER_DEFAULT = ROTATION_ORDER_ZXY
}; 


//-----------------------------------------------------------------------------------------------
struct Matrix44
{
public: 
	// first 4 elements represents the 
	union
	{
		float m_values[16];
		Vec4 m_cols[4];
	};

	enum // use to index into the matrix to get specific elements
	{
		Ix, Iy, Iz, Iw,		Jx, Jy, Jz, Jw,		Kx, Ky, Kz, Kw,		Tx, Ty, Tz, Tw
	};

	const static Matrix44 IDENTITY;

public:
	Matrix44();
	~Matrix44();
	explicit Matrix44( const Vec2& iVector, const Vec2& jVector, const Vec2& tVector );
	explicit Matrix44( const Vec3& iVector, const Vec3& jVector, const Vec3& kVector, const Vec3& tVector );
	explicit Matrix44( const Vec4& iVector, const Vec4& jVector, const Vec4 &kVector, const Vec4& tVector );
	explicit Matrix44( const float sixteenElementArray[16] );

	// Matrix constructors
 	static Matrix44		MakeXRotationDegrees( float degreesAboutX );
 	static Matrix44		MakeYRotationDegrees( float degreesAboutY );
	static Matrix44		MakeZRotationDegrees( float degreesAboutZ );
	static Matrix44		MakeRotationAboutAxis(const Vec3& axis, float angleDegrees);
	static Matrix44		MakeRotationFromDisplacementWithX( const Vec3& displacement );
	static Matrix44		MakeRotationFromDisplacementWithY( const Vec3& displacement );
	static Matrix44		MakeRotationFromDisplacementWithZ( const Vec3& displacement );
	static Matrix44		MakeTranslation2D( const Vec2& translationXY );
	static Matrix44		MakeTranslation3D( const Vec3& translationXY );
	static Matrix44		MakeUniformScale2D( float uniformScaleXY );
	static Matrix44		MakeUniformScale3D( float uniformScaleXY );
	static Matrix44		MakeScale2D( const Vec2& uniformScaleXY );
	static Matrix44		MakeScale3D( const Vec3& uniformScaleXY );
	static Matrix44		MakeProjectionPerspective( float fovDegrees, float aspect, float nearZ, float farZ );
	static Matrix44		MakeProjectionOrthographic( const Vec2& min, const Vec2& max, float nearZ, float farZ );
	static Matrix44		FromEuler( Vec3 euler, eRotationOrder rot );
	static Matrix44		LookAt( const Vec3& start, const Vec3& end, const Vec3& upRef );
	
public:
	Vec4 GetRow( int rowIndex ) const;
	Vec4 GetCol( int colIndex ) const;

	Vec4 GetI() const { return m_cols[0]; }
	Vec4 GetJ() const { return m_cols[1]; }
	Vec4 GetK() const { return m_cols[2]; }
	Vec4 GetT() const { return m_cols[3]; }

public:
	void SetI( const Vec4& values );
	void SetJ( const Vec4& values );
	void SetK( const Vec4& values );
	void SetT( const Vec4& values );
	void SetCol( const Vec4& values, unsigned int colIndex );

	void SetI( const Vec3& values );
	void SetJ( const Vec3& values );
	void SetK( const Vec3& values );
	void SetT( const Vec3& values );
	void SetCol( const Vec3& values, unsigned int colIndex );

	void Set( const Matrix44& mat );

public:
	const	Vec2 TransformVec2D( const Vec2& vectorQuantity ) const;	// Assumes Z=0, w=0
	const	Vec2 TransformPosition2D( const Vec2& position ) const;		// Assumes z=0, w=1

	const	Vec3 TransformVec3D( const Vec3& vectorQuantity ) const;	// Assumes w=0
	const	Vec3 TransformPosition3D( const Vec3& position ) const;		// Assumes w=1

	const	Vec4 TransformHomoPoint3D( const Vec4& homogeneousPoint ) const;	// Assumes nothing

	const	Matrix44 Concatenate( const Matrix44& other ) const;

public:
	void MakeIdentity();
	void Transpose();
	void InvertOrthonormal();
	void Invert();
	Matrix44 GetInverted() const;

public:
	void RotateAroundX( float degrees );
	void RotateAroundY( float degrees );
	void RotateAroundZ( float degrees );
	void Translate( Vec3 offset );

public:
	const Matrix44 operator*( Matrix44 concate ) const;			
	const Vec4 operator*( const Vec4& vector ) const;			
	const Matrix44 operator*( float scale ) const;			

};

void TransformPoints( uint count, Vec3* points, const Matrix44& matrix );

//--------------------------------------------------------------------------
/**
* GetTransfromFromString
* Assumes that there are 3 strings within string that will represent teh state of the transform.
* e.g. -z x -y
*/
Matrix44 GetTransfromFromString(const std::string& string);