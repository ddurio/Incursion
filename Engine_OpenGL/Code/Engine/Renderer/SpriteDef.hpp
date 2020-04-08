#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"


class SpriteDef {
    public:
    explicit SpriteDef( const Vec2& uvAtBottomLeft, const Vec2& uvAtTopRight );

    void GetUVs( Vec2& out_uvAtBottomLeft, Vec2& out_uvAtTopRight ) const;

    private:
    Vec2 m_uvAtBottomLeft = Vec2( 0.f, 0.f );
    Vec2 m_uvAtTopRight = Vec2( 1.f, 1.f );
};