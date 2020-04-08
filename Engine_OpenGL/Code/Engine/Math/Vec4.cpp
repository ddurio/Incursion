#include "Engine/Math/Vec4.hpp"


Vec4::Vec4( float inX, float inY, float inZ, float inW ) {
    x = inX;
    y = inY;
    z = inZ;
    w = inW;
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator + ( const Vec4& vecToAdd ) const {
	float tempX = this->x + vecToAdd.x;
	float tempY = this->y + vecToAdd.y;
	float tempZ = this->z + vecToAdd.z;
	float tempW = this->w + vecToAdd.w;
	return Vec4( tempX, tempY, tempZ, tempW );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const {
	float tempX = this->x - vecToSubtract.x;
	float tempY = this->y - vecToSubtract.y;
	float tempZ = this->z - vecToSubtract.z;
	float tempW = this->w - vecToSubtract.w;
	return Vec4( tempX, tempY, tempZ, tempW );
}

//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( float uniformScale ) const {
	float tempX = this->x * uniformScale;
	float tempY = this->y * uniformScale;
	float tempZ = this->z * uniformScale;
	float tempW = this->w * uniformScale;
	return Vec4( tempX, tempY, tempZ, tempW );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/( float inverseScale ) const {
	float tempX = this->x / inverseScale;
	float tempY = this->y / inverseScale;
	float tempZ = this->z / inverseScale;
	float tempW = this->w / inverseScale;
	return Vec4( tempX, tempY, tempZ, tempW );
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=( const Vec4& vecToAdd ) {
	this->x += vecToAdd.x;
	this->y += vecToAdd.y;
	this->z += vecToAdd.z;
	this->w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=( const Vec4& vecToSubtract ) {
	this->x -= vecToSubtract.x;
	this->y -= vecToSubtract.y;
	this->z -= vecToSubtract.z;
	this->w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=( const float uniformScale ) {
	this->x *= uniformScale;
	this->y *= uniformScale;
	this->z *= uniformScale;
	this->w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=( const float uniformDivisor ) {
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
	this->z /= uniformDivisor;
	this->w /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( const Vec4& copyFrom ) {
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
	this->w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
const Vec4 operator*( float uniformScale, const Vec4& vecToScale ) {
    return vecToScale * uniformScale;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( const Vec4& compare ) const {
	return (
        (this->x == compare.x) &&
        (this->y == compare.y) &&
        (this->z == compare.z) &&
        (this->w == compare.w) 
    );
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=( const Vec4& compare ) const {
	return (
        (this->x != compare.x) ||
        (this->y != compare.y) ||
        (this->z != compare.z) ||
        (this->w != compare.w) 
    );
}


const std::string Vec4::GetAsString() const {
    return Stringf( "%f,%f,%f,%f", x, y, z, w );
}


void Vec4::SetFromText( const char* valuesAsText ) {
    std::vector<std::string> splitString = SplitStringOnDelimeter( valuesAsText, ',' );

    int numValues = (int)splitString.size();
    GUARANTEE_OR_DIE( numValues == 4, Stringf( "Invalid initialization string (%s) for Vec4", valuesAsText ) );

    x = (float)atof( splitString[0].c_str() );
    y = (float)atof( splitString[1].c_str() );
    z = (float)atof( splitString[2].c_str() );
    w = (float)atof( splitString[3].c_str() );
}


