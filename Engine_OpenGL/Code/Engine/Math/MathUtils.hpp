#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct AABB2;
struct IntRange;
struct FloatRange;
struct Vec2;
struct Vec3;
struct Vec4;

constexpr float fSQRT_3_OVER_3 = 0.57735026918962576450914878050196f;

float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float degrees );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( float y, float x );

float GetDistanceXY( const Vec3& positionA, const Vec3& positionB );
float GetDistanceSquared( const Vec2& positionA, const Vec2& positionB );
float GetDistanceSquared( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXYSquared( const Vec3& positionA, const Vec3& positionB );

float GetDistance( const Vec2& positionA, const Vec2& positionB );
float GetDistance( const Vec3& positionA, const Vec3& positionB );

bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB );
bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB );

Vec2 TransformPosition( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation );
Vec3 TransformPosition( const Vec3& position, float scaleXY, float rotationDegreesAboutZ, const Vec2& translationXY );

float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float ClampFloat( float inValue, float minValue, float maxValue );
int ClampInt( int inValue, int minValue, int maxValue );

bool IsFloatInRange( float inValue, const FloatRange& range );
bool IsIntInRange( int inValue, const IntRange& range );


// Disc Physics
float GetAngulaDisplacement( float startDegrees, float endDegrees );
float GetTurnedTowards( float currentDegrees, float goalDegrees, float maxDeltaDegrees );

bool DoesDiscOverlapAABB2( const Vec2& discCenter, float discRadius, const AABB2& aabb2 );
const Vec2 GetClosestPointOnDisc( const Vec2& point, const Vec2& discCenter, float discRadius );

void PushDiscOutOfAABB2( Vec2& discCenter, float discRadius, const AABB2& aabb2 );
void PushDiscOutOfDisc( Vec2& discCenterToPush, float discRadiusA, const Vec2& discCenterToPushOutOf, float discRadiusB );
void PushDiscsOutOfEachOther( Vec2& centerA, float radiusA, Vec2& centerB, float radiusB );


// Dot Products
float DotProduct( const Vec2& vecA, const Vec2& vecB );
float DotProduct( const Vec3& vecA, const Vec3& vecB );
float DotProduct( const Vec4& vecA, const Vec4& vecB );

float GetProjectedLength( const Vec2& projected, const Vec2& onto );
const Vec2 GetProjectedVector( const Vec2& projected, const Vec2& onto );
const Vec2 GetReflectedVector( const Vec2& projected, const Vec2& onto );


// Easing Functions
float SmoothStart2( float inZeroToOne );
float SmoothStart3( float inZeroToOne );
float SmoothStart4( float inZeroToOne );
float SmoothStart5( float inZeroToOne );

float SmoothStop2( float inZeroToOne );
float SmoothStop3( float inZeroToOne );
float SmoothStop4( float inZeroToOne );
float SmoothStop5( float inZeroToOne );

float SmoothStep3( float inZeroToOne );
float SmoothStep5( float inZeroToOne );
