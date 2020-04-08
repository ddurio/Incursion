#include "Engine/Math/Vec2.hpp"

#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"


const Vec2 Vec2::ZERO = Vec2( 0.f, 0.f );
const Vec2 Vec2::ONE = Vec2( 1.f, 1.f );

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y ) {
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY ) {
}


Vec2::Vec2( const char* initialValues ) {
    SetFromText( initialValues );
}


const Vec2 Vec2::MakeFromPolarDegrees( float angleDegrees, float radius /*= 1.f */ ) {
    float tempX = radius * CosDegrees( angleDegrees );
    float tempY = radius * SinDegrees( angleDegrees );
    return Vec2( tempX, tempY );
}


const Vec2 Vec2::MakeFromPolarRadians( float angleRadians, float radius /*= 1.f */ ) {
    float tempX = radius * cos( angleRadians );
    float tempY = radius * sin( angleRadians );
    return Vec2( tempX, tempY );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const {
	float tempX = x + vecToAdd.x;
	float tempY = y + vecToAdd.y;
	return Vec2( tempX, tempY );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const {
	float tempX = x - vecToSubtract.x;
	float tempY = y - vecToSubtract.y;
	return Vec2( tempX, tempY );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const {
	float tempX = x * uniformScale;
	float tempY = y * uniformScale;
	return Vec2( tempX, tempY );
}


const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const {
    float tempX = x * vecToMultiply.x;
    float tempY = y * vecToMultiply.y;
    return Vec2( tempX, tempY );
}


const std::string Vec2::GetAsString() const {
    return Stringf( "%f,%f", x, y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const {
	float tempX = x / inverseScale;
	float tempY = y / inverseScale;
	return Vec2( tempX, tempY );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd ) {
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract ) {
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale ) {
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor ) {
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom ) {
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale ) {
	return Vec2( uniformScale * vecToScale.x, uniformScale * vecToScale.y );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const {
	return ((x == compare.x) && (y == compare.y));
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const {
	return ((x != compare.x) || (y != compare.y));
}


float Vec2::GetLength() const {
    return sqrt( GetLengthSquared() );
}


float Vec2::GetLengthSquared() const {
    return (x*x) + (y*y);
}


float Vec2::GetAngleDegrees() const {
    return Atan2Degrees( y, x );
}


float Vec2::GetAngleRadians() const {
    return atan2f( y, x );
}


const Vec2 Vec2::GetRotated90Degrees() const {
    return Vec2( -y, x );
}


const Vec2 Vec2::GetRotatedMinus90Degrees() const {
    return Vec2( y, -x );
}


const Vec2 Vec2::GetRotatedDegrees( float angleDegrees ) const {
    float theta = GetAngleDegrees();
    theta += angleDegrees;
    float tempX = GetLength() * CosDegrees( theta );
    float tempY = GetLength() * SinDegrees( theta );
    return Vec2( tempX, tempY );
}


const Vec2 Vec2::GetRotatedRadians( float angleRadians ) const {
    float theta = GetAngleRadians();
    theta += angleRadians;
    float tempX = GetLength() * cos( theta );
    float tempY = GetLength() * sin( theta );
    return Vec2( tempX, tempY );
}


const Vec2 Vec2::GetClamped( float maxLength ) const {
    float theta = GetAngleRadians();
    float radius = GetLength();
    radius = (radius > maxLength) ? maxLength : radius;
    return MakeFromPolarRadians( theta, radius );
}


const Vec2 Vec2::GetNormalized() const {
    float theta = GetAngleRadians();
    return MakeFromPolarRadians( theta, 1.f );
}


void Vec2::SetFromText( const char* valuesAsText ) {
    std::vector<std::string> splitString = SplitStringOnDelimeter( valuesAsText, ',' );

    int numValues = (int)splitString.size();
    GUARANTEE_OR_DIE( numValues == 2, Stringf( "Invalid initialization string (%s) for Vec2", valuesAsText ) );

    x = (float)atof( splitString[0].c_str() );
    y = (float)atof( splitString[1].c_str() );
}


void Vec2::ClampLength( float maxLength ) {
    float lengthSquared = GetLengthSquared();
    if( lengthSquared > (maxLength * maxLength) ) {
        SetLength( maxLength );
    }
}


void Vec2::SetLength( float newLength ) {
    Vec2 tempVec = MakeFromPolarRadians( GetAngleRadians(), newLength );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::SetAngleDegrees( float newAngleDegrees ) {
    float length = GetLength();
    Vec2 tempVec = MakeFromPolarDegrees( newAngleDegrees, length );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::SetAngleRadians( float newAngleRadians ) {
    float length = GetLength();
    Vec2 tempVec = MakeFromPolarRadians( newAngleRadians, length );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength ) {
    Vec2 tempVec = MakeFromPolarDegrees( newAngleDegrees, newLength );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::SetPolarRadians( float newangleRadians, float newLength ) {
    Vec2 tempVec = MakeFromPolarRadians( newangleRadians, newLength );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::RotateDegrees( float angleDegrees ) {
    Vec2 tempVec = GetRotatedDegrees( angleDegrees );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::RotateRadians( float angleRadians ) {
    Vec2 tempVec = GetRotatedRadians( angleRadians );
    x = tempVec.x;
    y = tempVec.y;
}


void Vec2::Rotate90Degrees() {
    RotateDegrees( 90.f );
}


void Vec2::RotateMinus90Degrees() {
    RotateDegrees( -90.f );
}


void Vec2::Normalize() {
    Vec2 tempVec = GetNormalized();
    x = tempVec.x;
    y = tempVec.y;
}


float Vec2::NormalizeGetPreviousLength() {
    float previousLength = GetLength();
    Normalize();
    return previousLength;
}
