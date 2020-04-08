#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <cmath>


//-----------------------------------------------------------------------------------------------
struct Vec3 {
	public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	// Construction/Destruction
	Vec3() {}												// default constructor: do nothing (for speed)
	Vec3( const Vec3& copyFrom );							// copy constructor (from another vec3)
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y, z)
    explicit Vec3( const char* initialValues );
	~Vec3() {}												// destructor: do nothing (for speed)

    static const Vec3 ZERO;
    static const Vec3 ONE;

	// Operators
	const Vec3 operator+( const Vec3& vecToAdd ) const;			// vec3 + vec3
	const Vec3 operator-( const Vec3& vecToSubtract ) const;	// vec3 - vec3
	const Vec3 operator*( float uniformScale ) const;			// vec3 * float
	const Vec3 operator/( float inverseScale ) const;			// vec3 / float
	void operator+=( const Vec3& vecToAdd );					// vec3 += vec3
	void operator-=( const Vec3& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );				// vec3 *= float
	void operator/=( const float uniformDivisor );				// vec3 /= float
	void operator=( const Vec3& copyFrom );						// vec3 = vec3
	bool operator==( const Vec3& compare ) const;				// vec3 == vec3
	bool operator!=( const Vec3& compare ) const;				// vec3 != vec3

	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3

    const std::string GetAsString() const;
    float GetLength() const;
    float GetLengthXY() const;
    float GetLengthSquared() const;
    float GetLengthXYSquared() const;
    float GetAngleAboutZDegrees() const;
    float GetAngleAboutZRadians() const;
    const Vec3 GetRotatedAboutZDegrees( float rotationDegrees ) const;
    const Vec3 GetRotatedAboutZRadians( float rotationRadians ) const;

    void SetFromText( const char* valuesAsText );
};
