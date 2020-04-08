#pragma once


struct FloatRange;
struct IntRange;

class RNG {
    public:
    explicit RNG();
    explicit RNG( unsigned int seed );

    void SetSeed( unsigned int seed );
    void SetPosition( unsigned int position );

    int GetRandomIntLessThan( int maxNotInclusive );
    int GetRandomIntInRange( int minInclusive, int maxInclusive );
    int GetRandomIntInRange( IntRange rangeInclusive );
    float GetRandomFloatZeroToOne();
    float GetRandomFloatInRange( float minInclusive, float maxInclusive );
    float GetRandomFloatInRange( FloatRange rangeInclusive );

    bool PercentChance( float fractionChance );

    protected:
    unsigned int m_seed = 0;
    unsigned int m_position = 0;

    static unsigned int s_defaultSeed;
};
