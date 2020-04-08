#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <cmath>


struct IntVec2;

//-----------------------------------------------------------------------------------------------
struct Vec2 {
	public:
	float x = 0.f;
	float y = 0.f;

	// Construction/Destruction
	Vec2() {}												// default constructor: do nothing (for speed)
	Vec2( const Vec2& copyFrom );							// copy constructor (from another vec2)
	explicit Vec2( float initialX, float initialY );		// explicit constructor (from x, y)
    explicit Vec2( const char* initialValues );
	~Vec2() {}												// destructor: do nothing (for speed)

    static const Vec2 ZERO;
    static const Vec2 ONE;

    static const Vec2 MakeFromPolarDegrees( float angleDegrees, float radius = 1.f );
    static const Vec2 MakeFromPolarRadians( float angleRadians, float radius = 1.f );

	// Operators
	const Vec2 operator+( const Vec2& vecToAdd ) const;			// Vec2 +  Vec2
	const Vec2 operator-( const Vec2& vecToSubtract ) const;	// Vec2 -  Vec2
	const Vec2 operator*( float uniformScale ) const;			// Vec2 *  float
	const Vec2 operator/( float inverseScale ) const;			// Vec2 /  float
    const Vec2 operator*( const Vec2& vecToMultiply ) const;    // Vec2 *  Vec2
    const Vec2 operator*( IntVec2 vecToMultiply ) const;        // Vec2 *  IntVec2
	void operator+=( const Vec2& vecToAdd );					// Vec2 += Vec2
	void operator-=( const Vec2& vecToSubtract );				// Vec2 -= Vec2
	void operator*=( const float uniformScale );				// Vec2 *= float
	void operator/=( const float uniformDivisor );				// Vec2 /= float
	void operator=( const Vec2& copyFrom );						// Vec2 =  Vec2
	bool operator==( const Vec2& compare ) const;				// Vec2 == Vec2
	bool operator!=( const Vec2& compare ) const;				// Vec2 != Vec2

	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2

    const std::string GetAsString() const;

	float GetLength() const;
    float GetLengthSquared() const;
    float GetAngleDegrees() const;
    float GetAngleRadians() const;
    const Vec2 GetRotated90Degrees() const;
    const Vec2 GetRotatedMinus90Degrees() const;
    const Vec2 GetRotatedDegrees( float angleDegrees ) const;
    const Vec2 GetRotatedRadians( float angleRadians ) const;
    const Vec2 GetClamped( float maxLength ) const;
    const Vec2 GetNormalized() const;

    void SetFromText( const char* valuesAsText );
    void ClampLength( float maxLength );
    void SetLength( float newLength );
    void SetAngleDegrees( float newAngleDegrees );
    void SetAngleRadians( float newAngleRadians );
    void SetPolarDegrees( float newAngleDegrees, float newLength );
    void SetPolarRadians( float newangleRadians, float newLength );
    void RotateDegrees( float angleDegrees );
    void RotateRadians( float angleRadians );
    void Rotate90Degrees();
    void RotateMinus90Degrees();
    void Normalize();
    float NormalizeGetPreviousLength();
};
