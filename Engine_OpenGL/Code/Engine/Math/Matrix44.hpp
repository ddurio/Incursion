#pragma once
#include "Engine/Core/EngineCommon.hpp"


struct Vec2;
struct Vec3;
struct Vec4;

enum {
    Ix, Iy, Iz, Iw,    Jx, Jy, Jz, Jw,    Kx, Ky, Kz, Kw,    Tx, Ty, Tz, Tw
};

struct Matrix44 {
    public:
    float m_values[16] = { 0 };

    Matrix44();
    Matrix44( float* valuesBasisMajor );
    Matrix44( const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation );
    Matrix44( const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& translation );
    Matrix44( const Vec4& iHomogeneous, const Vec4& jHomogeneous, const Vec4& kHomogeneous, const Vec4& translationHomogeneous );
    ~Matrix44() {};

    static Matrix44 MakeZRotationDegrees( float degrees );
    static Matrix44 MakeTranslation2D( const Vec2& translationXY );
    static Matrix44 MakeUniformScale2D( float uniformScaleXY );

    float  operator[]( int index ) const; // Getter
    float& operator[]( int index );      // Setter

    // 2D
    const Vec2 TransformVector2D( const Vec2& vectorQuantity ) const;
    const Vec2 TransformPosition2D( const Vec2& position ) const;

    // 3D
    const Vec3 TransformVector3D( const Vec3& vectorQuantity ) const;
    const Vec3 TransformPosition3D( const Vec3& position ) const;
    const Vec4 TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const;
};
