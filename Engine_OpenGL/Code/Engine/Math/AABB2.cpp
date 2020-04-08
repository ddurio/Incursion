#include "Engine/Math/AABB2.hpp"

#include "Engine/Math/MathUtils.hpp"


const AABB2 AABB2::ZEROTOONE = AABB2( Vec2::ZERO, Vec2::ONE );

AABB2::AABB2( const AABB2& copyFrom ) {
    mins = copyFrom.mins;
    maxs = copyFrom.maxs;
}


AABB2::AABB2( Vec2 initialMins, Vec2 initialMaxs ) {
    mins = initialMins;
    maxs = initialMaxs;
}


const AABB2 AABB2::operator+( const AABB2& aabb2ToAdd ) const {
    Vec2 tempMins = mins + aabb2ToAdd.mins;
    Vec2 tempMaxs = maxs + aabb2ToAdd.maxs;
    return AABB2( tempMins, tempMaxs );
}


const AABB2 AABB2::operator-( const AABB2& aabb2ToSubtract ) const {
    Vec2 tempMins = mins - aabb2ToSubtract.mins;
    Vec2 tempMaxs = maxs - aabb2ToSubtract.maxs;
    return AABB2( tempMins, tempMaxs );
}


const AABB2 AABB2::operator*( float uniformScale ) const {
    Vec2 tempMins = mins * uniformScale;
    Vec2 tempMaxs = maxs * uniformScale;
    return AABB2( tempMins, tempMaxs );
}


const AABB2 AABB2::operator/( float inverseScale ) const {
    Vec2 tempMins = mins / inverseScale;
    Vec2 tempMaxs = maxs / inverseScale;
    return AABB2( tempMins, tempMaxs );
}


const AABB2 AABB2::operator+( const Vec2& vecToAdd ) const {
    Vec2 tempMins = mins + vecToAdd;
    Vec2 tempMaxs = maxs + vecToAdd;
    return AABB2( tempMins, tempMaxs );
}


const AABB2 AABB2::operator-( const Vec2& vecToSubtract ) const {
    Vec2 tempMins = mins - vecToSubtract;
    Vec2 tempMaxs = maxs - vecToSubtract;
    return AABB2( tempMins, tempMaxs );
}


void AABB2::operator+=( const AABB2& aabb2ToAdd ) {
    mins += aabb2ToAdd.mins;
    maxs += aabb2ToAdd.maxs;
}


void AABB2::operator-=( const AABB2& aabb2ToSubtract ) {
    mins -= aabb2ToSubtract.mins;
    maxs -= aabb2ToSubtract.maxs;
}


void AABB2::operator*=( const float uniformScale ) {
    mins *= uniformScale;
    maxs *= uniformScale;
}


void AABB2::operator/=( const float uniformDivisor ) {
    mins /= uniformDivisor;
    maxs /= uniformDivisor;
}


void AABB2::operator=( const AABB2& copyFrom ) {
    mins = copyFrom.mins;
    maxs = copyFrom.maxs;
}


bool AABB2::operator==( const AABB2& compare ) const {
    bool sameMins = (mins == compare.mins);
    bool sameMaxs = (mins == compare.maxs);
    return (sameMins && sameMaxs);
}


bool AABB2::operator!=( const AABB2& compare ) const {
    bool sameMins = (mins == compare.mins);
    bool sameMaxs = (mins == compare.maxs);
    return !(sameMins && sameMaxs);
}


const AABB2 operator*( float uniformScale, const AABB2& aabb2ToScale ) {
    Vec2 tempMins = aabb2ToScale.mins * uniformScale;
    Vec2 tempMaxs = aabb2ToScale.maxs * uniformScale;
    return(AABB2( tempMins, tempMaxs ));
}


const AABB2 operator+( const Vec2& vecToAdd, const AABB2& aabb2ToAdd ) {
    Vec2 tempMins = aabb2ToAdd.mins + vecToAdd;
    Vec2 tempMaxs = aabb2ToAdd.maxs + vecToAdd;
    return AABB2( tempMins, tempMaxs );
}


bool AABB2::IsPointInside( const Vec2& point ) const {
    Vec2 closestPoint = GetClosestPointOnAABB2( point );
    return (closestPoint == point);
}


const Vec2 AABB2::GetClosestPointOnAABB2( const Vec2& point ) const {
    float closestX = ClampFloat( point.x, mins.x, maxs.x );
    float closestY = ClampFloat( point.y, mins.y, maxs.y );
    return Vec2( closestX, closestY );
}


const Vec2 AABB2::GetDimensions() const {
    float lenghtX = maxs.x - mins.x;
    float lenghtY = maxs.y - mins.y;
    return Vec2( lenghtX, lenghtY );
}


const AABB2 AABB2::GetPaddedAABB2( float padding ) const {
    Vec2 paddingVec = Vec2( padding, padding );
    Vec2 tempMins = mins - paddingVec;
    Vec2 tempMaxs = maxs + paddingVec;
    return AABB2( tempMins, tempMaxs );
}


void AABB2::GrowToIncludePoint( const Vec2& pointToInclude ) {
    if( !IsPointInside(pointToInclude) ) {
        if( pointToInclude.x < mins.x ) {
            mins.x = pointToInclude.x;
        } else if( pointToInclude.x > maxs.x ) {
            maxs.x = pointToInclude.x;
        }

        if( pointToInclude.y < mins.y ) {
            mins.y = pointToInclude.y;
        } else if( pointToInclude.y > maxs.y ) {
            maxs.y = pointToInclude.y;
        }
    }
}


void AABB2::AlignWithinBox( const AABB2& box, const Vec2& alignment ) {
    Vec2 boxDimensions = box.GetDimensions();
    Vec2 myDimensions = GetDimensions();

    Vec2 spareSpace = boxDimensions - myDimensions;
    Vec2 localSpace = spareSpace * alignment; // position relative to bounding box
    mins = box.mins + localSpace;
    maxs = mins + myDimensions;
}


AABB2 AABB2::GetBoxWithin( const Vec2& boxDimensions, const Vec2& alignment ) const {
    Vec2 myDimensions = GetDimensions();

    Vec2 spareSpace = myDimensions - boxDimensions;
    Vec2 localSpace = spareSpace * alignment; // position relative to bounding box

    Vec2 boxMins = mins + localSpace;
    Vec2 boxMaxs = boxMins + boxDimensions;


    //Vec2 spareSpace = boxDimensions - textDimensions;
    //Vec2 localSpace = spareSpace * alignment; // box's position relative to bounding box
    //Vec2 startPos = boxBounds.mins + localSpace;




    return AABB2( boxMins, boxMaxs );
}


AABB2 AABB2::CarveBoxOffBottom( float fractionOfHeight, float additionalHeight /*= 0.f */ ) {
    Vec2 myDimensions = GetDimensions();
    float boxWidth = myDimensions.x;
    float boxHeight = myDimensions.y * fractionOfHeight + additionalHeight;

    Vec2 boxDimensions = Vec2( boxWidth, boxHeight );
    Vec2 boxMins = mins;
    Vec2 boxMaxs = boxMins + boxDimensions;

    mins.y += boxHeight;

    return AABB2( boxMins, boxMaxs );
}


AABB2 AABB2::CarveBoxOffTop( float fractionOfHeight, float additionalHeight /*= 0.f */ ) {
    Vec2 myDimensions = GetDimensions();
    float boxWidth = myDimensions.x;
    float boxHeight = myDimensions.y * fractionOfHeight + additionalHeight;

    Vec2 boxDimensions = Vec2( boxWidth, boxHeight );
    Vec2 boxMaxs = maxs;
    Vec2 boxMins = boxMaxs - boxDimensions;

    maxs.y -= boxHeight;

    return AABB2( boxMins, boxMaxs );
}
