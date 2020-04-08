#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"


const Vec3 Vec3::ZERO = Vec3( 0.f, 0.f, 0.f );
const Vec3 Vec3::ONE = Vec3( 1.f, 1.f, 1.f );

//-----------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z ) {
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ ) {
}


Vec3::Vec3( const char* initialValues ) {
    SetFromText( initialValues );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const {
	float tempX = this->x + vecToAdd.x;
	float tempY = this->y + vecToAdd.y;
	float tempZ = this->z + vecToAdd.z;
	return Vec3( tempX, tempY, tempZ );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const {
	float tempX = this->x - vecToSubtract.x;
	float tempY = this->y - vecToSubtract.y;
	float tempZ = this->z - vecToSubtract.z;
	return Vec3( tempX, tempY, tempZ );
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const {
	float tempX = this->x * uniformScale;
	float tempY = this->y * uniformScale;
	float tempZ = this->z * uniformScale;
	return Vec3( tempX, tempY, tempZ );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const {
	float tempX = this->x / inverseScale;
	float tempY = this->y / inverseScale;
	float tempZ = this->z / inverseScale;
	return Vec3( tempX, tempY, tempZ );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd ) {
	this->x += vecToAdd.x;
	this->y += vecToAdd.y;
	this->z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract ) {
	this->x -= vecToSubtract.x;
	this->y -= vecToSubtract.y;
	this->z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale ) {
	this->x *= uniformScale;
	this->y *= uniformScale;
	this->z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor ) {
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
	this->z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom ) {
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale ) {
    return vecToScale * uniformScale;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const {
	return ((this->x == compare.x) && (this->y == compare.y) && (this->z == compare.z));
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const {
	return ((this->x != compare.x) || (this->y != compare.y) || (this->z != compare.z));
}


const std::string Vec3::GetAsString() const {
    return Stringf( "%f,%f,%f", x, y, z );
}


float Vec3::GetLength() const {
	return sqrt( GetLengthSquared() );
}


float Vec3::GetLengthXY() const {
    return sqrt( GetLengthXYSquared() );
}


float Vec3::GetLengthSquared() const {
    return (x*x) + (y*y) + (z*z);
}


float Vec3::GetLengthXYSquared() const {
    return (x*x) + (y*y);
}


float Vec3::GetAngleAboutZDegrees() const {
    return Atan2Degrees( y, x );
}


float Vec3::GetAngleAboutZRadians() const {
    return atan2f( y, x );
}


const Vec3 Vec3::GetRotatedAboutZDegrees( float rotationDegrees ) const {
    float angleDegrees = GetAngleAboutZDegrees();
    angleDegrees += rotationDegrees;
    float tempX = GetLengthXY() * CosDegrees( angleDegrees );
    float tempY = GetLengthXY() * SinDegrees( angleDegrees );
    return Vec3( tempX, tempY, z );
}


const Vec3 Vec3::GetRotatedAboutZRadians( float rotationRadians ) const {
    float angleRadians = GetAngleAboutZRadians();
    angleRadians += rotationRadians;
    float tempX = GetLengthXY() * cos( angleRadians );
    float tempY = GetLengthXY() * sin( angleRadians );
    return Vec3( tempX, tempY, z );
}


void Vec3::SetFromText( const char* valuesAsText ) {
    std::vector<std::string> splitString = SplitStringOnDelimeter( valuesAsText, ',' );

    int numValues = (int)splitString.size();
    GUARANTEE_OR_DIE( numValues == 3, Stringf( "Invalid initialization string (%s) for Vec3", valuesAsText ) );

    x = (float)atof( splitString[0].c_str() );
    y = (float)atof( splitString[1].c_str() );
    z = (float)atof( splitString[2].c_str() );
}
