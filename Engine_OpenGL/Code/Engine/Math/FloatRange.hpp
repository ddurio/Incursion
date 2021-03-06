#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct FloatRange {
    public:
    float min;
    float max;

    explicit FloatRange( float initialMin, float initialMax );
    explicit FloatRange( const char* initialRange );
    ~FloatRange() {};

    static const FloatRange ZERO;
    static const FloatRange ZEROTOONE;
    static const FloatRange NEGONE;

    const std::string GetAsString() const;
    void SetFromText( const char* valuesAsText );

    // Operators
    const FloatRange operator+( const FloatRange& rangeToAdd ) const;		// FloatRange +  FloatRange
    const FloatRange operator-( const FloatRange& rangeToSubtract ) const;	// FloatRange -  FloatRange
    const FloatRange operator*( float uniformScale ) const;			        // FloatRange *  int
    const FloatRange operator/( float inverseScale ) const;			        // FloatRange /  int
    void operator*=( const float uniformScale );		                    // FloatRange *= float
    void operator/=( const float uniformDivisor );		                    // FloatRange /= float
    void operator=(  const FloatRange& copyFrom );		                    // FloatRange =  FloatRange
    bool operator==( const FloatRange& compare ) const;	                    // FloatRange == FloatRange
    bool operator!=( const FloatRange& compare ) const;	                    // FloatRange != FloatRange

    friend const FloatRange operator*( float uniformScale, const FloatRange& rangeToScale );	// float * range2
};
