#include "Engine/Math/Matrix44.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


Matrix44::Matrix44() {
    // Set to identity
    m_values[Ix] = 1;
    m_values[Jy] = 1;
    m_values[Kz] = 1;
    m_values[Tw] = 1;
}


Matrix44::Matrix44( float* valuesBasisMajor ) {
    for( int index = 0; index < 16; index++ ) {
        m_values[index] = valuesBasisMajor[index];
    }
}


Matrix44::Matrix44( const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation ) {
    m_values[Ix] = iBasis.x;
    m_values[Iy] = iBasis.y;

    m_values[Jx] = jBasis.x;
    m_values[Jy] = jBasis.y;

    m_values[Kz] = 1;

    m_values[Tx] = translation.x;
    m_values[Ty] = translation.y;
    m_values[Tw] = 1;
}


Matrix44::Matrix44( const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& translation ) {
    m_values[Ix] = iBasis.x;
    m_values[Iy] = iBasis.y;
    m_values[Iz] = iBasis.z;

    m_values[Jx] = jBasis.x;
    m_values[Jy] = jBasis.y;
    m_values[Jz] = jBasis.z;

    m_values[Kx] = kBasis.x;
    m_values[Ky] = kBasis.y;
    m_values[Kz] = kBasis.z;

    m_values[Tx] = translation.x;
    m_values[Ty] = translation.y;
    m_values[Tz] = translation.z;
    m_values[Tw] = 1;
}


Matrix44::Matrix44( const Vec4& iHomogeneous, const Vec4& jHomogeneous, const Vec4& kHomogeneous, const Vec4& translationHomogeneous ) {
    m_values[Ix] = iHomogeneous.x;
    m_values[Iy] = iHomogeneous.y;
    m_values[Iz] = iHomogeneous.z;
    m_values[Iw] = iHomogeneous.w;

    m_values[Jx] = jHomogeneous.x;
    m_values[Jy] = jHomogeneous.y;
    m_values[Jz] = jHomogeneous.z;
    m_values[Jw] = jHomogeneous.w;

    m_values[Kx] = kHomogeneous.x;
    m_values[Ky] = kHomogeneous.y;
    m_values[Kz] = kHomogeneous.z;
    m_values[Kw] = kHomogeneous.w;

    m_values[Tx] = translationHomogeneous.x;
    m_values[Ty] = translationHomogeneous.y;
    m_values[Tz] = translationHomogeneous.z;
    m_values[Tw] = translationHomogeneous.w;
}


Matrix44 Matrix44::MakeZRotationDegrees( float degrees ) {
    Matrix44 rotationMatrix;

    float c = CosDegrees( degrees );
    float s = SinDegrees( degrees );

    rotationMatrix[Ix] = c;
    rotationMatrix[Iy] = s;

    rotationMatrix[Jx] = -s;
    rotationMatrix[Jy] = c;

    return rotationMatrix;
}


Matrix44 Matrix44::MakeTranslation2D( const Vec2& translationXY ) {
    Matrix44 translationMatrix;

    translationMatrix[Tx] = translationXY.x;
    translationMatrix[Ty] = translationXY.y;

    return translationMatrix;
}


Matrix44 Matrix44::MakeUniformScale2D( float uniformScaleXY ) {
    Matrix44 scaledMatrix;

    scaledMatrix[Ix] = uniformScaleXY;
    scaledMatrix[Jy] = uniformScaleXY;

    return scaledMatrix;
}


float Matrix44::operator[]( int index ) const {
    return m_values[index];
}


float& Matrix44::operator[]( int index ) {
    return m_values[index];
}


const Vec2 Matrix44::TransformVector2D( const Vec2& vectorQuantity ) const {
    float tempX = m_values[Ix] * vectorQuantity.x + m_values[Jx] * vectorQuantity.y;
    float tempY = m_values[Iy] * vectorQuantity.x + m_values[Jy] * vectorQuantity.y;

    return Vec2( tempX, tempY );
}


const Vec2 Matrix44::TransformPosition2D( const Vec2& position ) const {
    float tempX = m_values[Ix] * position.x + m_values[Jx] * position.y + m_values[Tx];
    float tempY = m_values[Iy] * position.x + m_values[Jy] * position.y + m_values[Ty];

    return Vec2( tempX, tempY );
}


const Vec3 Matrix44::TransformVector3D( const Vec3& vectorQuantity ) const {
    float tempX = m_values[Ix] * vectorQuantity.x + m_values[Jx] * vectorQuantity.y + m_values[Kx] * vectorQuantity.z;
    float tempY = m_values[Iy] * vectorQuantity.x + m_values[Jy] * vectorQuantity.y + m_values[Ky] * vectorQuantity.z;
    float tempZ = m_values[Iz] * vectorQuantity.x + m_values[Jz] * vectorQuantity.y + m_values[Kz] * vectorQuantity.z;

    return Vec3( tempX, tempY, tempZ );
}


const Vec3 Matrix44::TransformPosition3D( const Vec3& position ) const {
    float tempX = m_values[Ix] * position.x + m_values[Jx] * position.y + m_values[Kx] * position.z + m_values[Tx];
    float tempY = m_values[Iy] * position.x + m_values[Jy] * position.y + m_values[Ky] * position.z + m_values[Ty];
    float tempZ = m_values[Iz] * position.x + m_values[Jz] * position.y + m_values[Kz] * position.z + m_values[Tz];

    return Vec3( tempX, tempY, tempZ );
}


const Vec4 Matrix44::TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const {
    float tempX = m_values[Ix] * homogeneousPoint.x + m_values[Jx] * homogeneousPoint.y + m_values[Kx] * homogeneousPoint.z + m_values[Tx] * homogeneousPoint.w;
    float tempY = m_values[Iy] * homogeneousPoint.x + m_values[Jy] * homogeneousPoint.y + m_values[Ky] * homogeneousPoint.z + m_values[Ty] * homogeneousPoint.w;
    float tempZ = m_values[Iz] * homogeneousPoint.x + m_values[Jz] * homogeneousPoint.y + m_values[Kz] * homogeneousPoint.z + m_values[Tz] * homogeneousPoint.w;
    float tempW = m_values[Iw] * homogeneousPoint.x + m_values[Jw] * homogeneousPoint.y + m_values[Kw] * homogeneousPoint.z + m_values[Tw] * homogeneousPoint.w;

    return Vec4( tempX, tempY, tempZ, tempW );
}
