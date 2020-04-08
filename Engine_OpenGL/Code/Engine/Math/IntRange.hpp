#pragma once
#include "Engine/Core/EngineCommon.hpp"


struct IntRange {
    public:
    int min;
    int max;

    explicit IntRange( int initialMin, int initialMax );
    explicit IntRange( const char* initialRange );
    ~IntRange() {};

    static const IntRange ZERO;
    static const IntRange ONE;

    const std::string GetAsString() const;
    void SetFromText( const char* valuesAsTExt );

    bool IsIntInRange( int inValue ) const;
};
