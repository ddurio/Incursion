#pragma once
#include "Engine/Core/EngineCommon.hpp"


struct Vec4 {
    public:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    Vec4( float inX, float inY, float inZ, float inW );

    // Operators
	const Vec4 operator+( const Vec4& vecToAdd ) const;			// vec4 + vec4
	const Vec4 operator-( const Vec4& vecToSubtract ) const;	// vec4 - vec4
	const Vec4 operator*( float uniformScale ) const;			// vec4 * float
	const Vec4 operator/( float inverseScale ) const;			// vec4 / float
	void operator+=( const Vec4& vecToAdd );					// vec4 += vec4
	void operator-=( const Vec4& vecToSubtract );				// vec4 -= vec4
	void operator*=( const float uniformScale );				// vec4 *= float
	void operator/=( const float uniformDivisor );				// vec4 /= float
	void operator=( const Vec4& copyFrom );						// vec4 = vec4
	bool operator==( const Vec4& compare ) const;				// vec4 == vec4
	bool operator!=( const Vec4& compare ) const;				// vec4 != vec4

	friend const Vec4 operator*( float uniformScale, const Vec4& vecToScale );	// float * vec4

    const std::string GetAsString() const;
    void SetFromText( const char* valuesAsText );
};
