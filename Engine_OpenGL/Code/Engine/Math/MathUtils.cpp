#define _USE_MATH_DEFINES
#include <cmath>
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


float ConvertRadiansToDegrees( float radians ) {
	return (radians * 180.f / (float)M_PI);
}


float ConvertDegreesToRadians( float degrees ) {
	return (degrees * (float)M_PI / 180.f);
}


float CosDegrees( float degrees ) {
	return cos( ConvertDegreesToRadians( degrees ) );
}


float SinDegrees( float degrees ) {
	return sin( ConvertDegreesToRadians( degrees ) );
}


float Atan2Degrees( float y, float x ) {
    return ConvertRadiansToDegrees( atan2f( y, x ) );
}


float GetDistanceXY( const Vec3& positionA, const Vec3& positionB ) {
    Vec3 displacement = positionB - positionA;
    return displacement.GetLengthXY();
}


float GetDistanceSquared( const Vec2& positionA, const Vec2& positionB ) {
    Vec2 displacement = positionB - positionA;
    return displacement.GetLengthSquared();
}


float GetDistanceSquared( const Vec3& positionA, const Vec3& positionB ) {
    Vec3 displacement = positionB - positionA;
    return displacement.GetLengthSquared();
}


float GetDistanceXYSquared( const Vec3& positionA, const Vec3& positionB ) {
    Vec3 displacement = positionB - positionA;
    return displacement.GetLengthXYSquared();
}


float GetDistance( const Vec2& positionA, const Vec2& positionB ) {
    Vec2 displacement = positionB - positionA;
    return displacement.GetLength();
}


float GetDistance( const Vec3& positionA, const Vec3& positionB ) {
    Vec3 displacement = positionB - positionA;
    return displacement.GetLength();
}


bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB ) {
    float distanceCenter = GetDistance( centerA, centerB );
    float distanceRadii = radiusA + radiusB;
    return (distanceCenter < distanceRadii);
}


bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB ) {
    float distanceCenter = GetDistance( centerA, centerB );
    float distanceRadii = radiusA + radiusB;
    return (distanceCenter < distanceRadii);
}


Vec2 TransformPosition( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation ) {
    // Scale
    Vec2 scaled = position * uniformScale;

    // Rotate
    Vec2 rotated = scaled.GetRotatedDegrees( rotationDegrees );

    // Translate
    Vec2 translated = rotated + translation;

    return translated;
}


Vec3 TransformPosition( const Vec3& position, float scaleXY, float rotationDegreesZ, const Vec2& translationXY ) {
    // Scale
    Vec3 scaled = Vec3( position.x * scaleXY, position.y * scaleXY, position.z );

    // Rotate
    Vec3 rotated = scaled.GetRotatedAboutZDegrees( rotationDegreesZ );

    // Translate
    Vec3 translated = rotated + Vec3( translationXY.x, translationXY.y, 0.0f );

    return translated;
}


float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd ) {
    if( inStart == inEnd ) {
        // Avoid divide by zero failure
        return (outStart + outEnd) * 0.5f;
    }

    float inRange = inEnd - inStart;
    float outRange = outEnd - outStart;

    float inFraction = (inValue - inStart) / inRange;
    float outValue = (inFraction * outRange) + outStart;

    return outValue;
}


float ClampFloat( float inValue, float minValue, float maxValue ) {
    if( inValue < minValue ) {
        return minValue;
    } else if( inValue < maxValue ) {
        return inValue;
    } else {
        return maxValue;
    }
}


int ClampInt( int inValue, int minValue, int maxValue ) {
    if( inValue < minValue ) {
        return minValue;
    } else if( inValue < maxValue ) {
        return inValue;
    } else {
        return maxValue;
    }
}


bool IsFloatInRange( float inValue, const FloatRange& range ) {
    return (inValue >= range.min && inValue <= range.max);
}


bool IsIntInRange( int inValue, const IntRange& range ) {
    return (inValue >= range.min && inValue <= range.max);
}


float GetAngulaDisplacement( float startDegrees, float endDegrees ) {
    float displacementDegrees = endDegrees - startDegrees;

    while( displacementDegrees > 180.f ) {
        displacementDegrees -= 360.f;
    }

    while( displacementDegrees < -180.f ) {
        displacementDegrees += 360.f;
    }

    return displacementDegrees;
}


float GetTurnedTowards( float currentDegrees, float goalDegrees, float maxDeltaDegrees ) {
    float displacementDegrees = GetAngulaDisplacement( currentDegrees, goalDegrees );
    float turnDegrees = ClampFloat( displacementDegrees, -maxDeltaDegrees, maxDeltaDegrees );
    return currentDegrees + turnDegrees;
}


bool DoesDiscOverlapAABB2( const Vec2& discCenter, float discRadius, const AABB2& aabb2 ) {
    Vec2 closestPoint = aabb2.GetClosestPointOnAABB2( discCenter );
    Vec2 displacement = (discCenter - closestPoint);
    float dispMagnitude = displacement.GetLength();
    return (dispMagnitude < discRadius);
}


const Vec2 GetClosestPointOnDisc( const Vec2& point, const Vec2& discCenter, float discRadius ) {
    Vec2 displacement = point - discCenter;
    displacement.ClampLength( discRadius );
    return displacement;
}


void PushDiscOutOfAABB2( Vec2& discCenter, float discRadius, const AABB2& aabb2 ) {
    Vec2 closestPoint = aabb2.GetClosestPointOnAABB2( discCenter );
    Vec2 displacement = (discCenter - closestPoint);
    float dispMagnitude = displacement.GetLength();

    if( dispMagnitude < discRadius ) {
        displacement.SetLength( discRadius - dispMagnitude );
        discCenter += displacement;
    }
}


void PushDiscOutOfDisc( Vec2& centerToPush, float radiusA, const Vec2& centerToPushOutOf, float radiusB ) {
    Vec2 displacement = (centerToPush - centerToPushOutOf);
    float dispMagnitude = displacement.GetLength();
    float combinedRadii = radiusA + radiusB;

    if( dispMagnitude < combinedRadii ) {
        float distanceToPushOut = combinedRadii - dispMagnitude;
        displacement.SetLength( distanceToPushOut );
        centerToPush += displacement;
    }
}


void PushDiscsOutOfEachOther( Vec2& centerA, float radiusA, Vec2& centerB, float radiusB ) {
    Vec2 displacement = (centerB - centerA);
    float dispMagnitude = displacement.GetLength();
    float combinedRadii = radiusA + radiusB;

    if( dispMagnitude < combinedRadii ) {
        float distanceToPushOut = combinedRadii - dispMagnitude;
        displacement.SetLength( 0.5f * distanceToPushOut );
        centerA -= displacement;
        centerB += displacement;
    }
}


float DotProduct( const Vec2& vecA, const Vec2& vecB ) {
    return (
        (vecA.x * vecB.x) +
        (vecA.y * vecB.y)
    );
}


float DotProduct( const Vec3& vecA, const Vec3& vecB ) {
    return (
        (vecA.x * vecB.x) +
        (vecA.y * vecB.y) +
        (vecA.z * vecB.z)
    );
}


float DotProduct( const Vec4& vecA, const Vec4& vecB ) {
    return (
        (vecA.x * vecB.x) +
        (vecA.y * vecB.y) +
        (vecA.z * vecB.z) +
        (vecA.w * vecB.w)
    );
}


float GetProjectedLength( const Vec2& projected, const Vec2& onto ) {
    Vec2 basisDirection = onto.GetNormalized();
    return DotProduct( projected, basisDirection );
}


const Vec2 GetProjectedVector( const Vec2& projected, const Vec2& onto ) {
    Vec2 basisDirection = onto.GetNormalized();
    float length = DotProduct( projected, basisDirection );
    return length * basisDirection;
}


const Vec2 GetReflectedVector( const Vec2& projected, const Vec2& onto ) {
    Vec2 normal = GetProjectedVector( projected, onto );
    Vec2 tangential = projected - normal;
    normal *= -1;
    return (normal + tangential);
}


float SmoothStart2( float inZeroToOne ) {
    return inZeroToOne * inZeroToOne;
}


float SmoothStart3( float inZeroToOne ) {
    return inZeroToOne * inZeroToOne * inZeroToOne;
}


float SmoothStart4( float inZeroToOne ) {
    return inZeroToOne * inZeroToOne * inZeroToOne * inZeroToOne;
}


float SmoothStart5( float inZeroToOne ) {
    return inZeroToOne * inZeroToOne * inZeroToOne * inZeroToOne * inZeroToOne;
}


float SmoothStop2( float inZeroToOne ) {
    float oneMinusIn = 1 - inZeroToOne;
    return 1 - (oneMinusIn * oneMinusIn);
}


float SmoothStop3( float inZeroToOne ) {
    float oneMinusIn = 1 - inZeroToOne;
    return 1 - (oneMinusIn * oneMinusIn * oneMinusIn);
}


float SmoothStop4( float inZeroToOne ) {
    float oneMinusIn = 1 - inZeroToOne;
    return 1 - (oneMinusIn * oneMinusIn * oneMinusIn * oneMinusIn);
}


float SmoothStop5( float inZeroToOne ) {
    float oneMinusIn = 1 - inZeroToOne;
    return 1 - (oneMinusIn * oneMinusIn * oneMinusIn * oneMinusIn * oneMinusIn);
}


float SmoothStep3( float inZeroToOne ) {
    return inZeroToOne * inZeroToOne * (3 - (2 * inZeroToOne));
}


float SmoothStep5( float inZeroToOne ) {
    const float& t = inZeroToOne;
    return t * t * t * (t * ((t * 6) - 15) + 10);
}
