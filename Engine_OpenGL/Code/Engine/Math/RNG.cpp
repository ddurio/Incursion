#include "Engine/Math/RNG.hpp"

#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/RawNoise.hpp"


unsigned int RNG::s_defaultSeed = 0;

RNG::RNG() {
    m_seed = s_defaultSeed++;
};


RNG::RNG( unsigned int seed ) :
    m_seed(seed) {
}


void RNG::SetSeed( unsigned int seed ) {
    m_seed = seed;
}


void RNG::SetPosition( unsigned int position ) {
    m_position = position;
}


int RNG::GetRandomIntLessThan( int maxNotInclusive ) {
    unsigned int random = Get1dNoiseUint( m_position++, m_seed );
    return random % maxNotInclusive;
}


int RNG::GetRandomIntInRange( int minInclusive, int maxInclusive ) {
    int range = maxInclusive - minInclusive + 1;
    unsigned int random = Get1dNoiseUint( m_position++, m_seed );

    return (random % range) + minInclusive;
}


int RNG::GetRandomIntInRange( IntRange rangeInclusive ) {
    return GetRandomIntInRange( rangeInclusive.min, rangeInclusive.max );
}


float RNG::GetRandomFloatZeroToOne() {
    return Get1dNoiseZeroToOne( m_position++, m_seed );
}


float RNG::GetRandomFloatInRange( float minInclusive, float maxInclusive ) {
    float range = maxInclusive - minInclusive;
    return (GetRandomFloatZeroToOne() * range) + minInclusive;
}


float RNG::GetRandomFloatInRange( FloatRange rangeInclusive ) {
    return GetRandomFloatInRange( rangeInclusive.min, rangeInclusive.max );
}


bool RNG::PercentChance( float fractionChance ) {
    float chanceRoll = GetRandomFloatZeroToOne();
    return (chanceRoll <= fractionChance);
}
