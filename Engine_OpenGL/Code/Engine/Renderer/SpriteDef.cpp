#include "Engine/Renderer/SpriteDef.hpp"


SpriteDef::SpriteDef( const Vec2& uvAtBottomLeft, const Vec2& uvAtTopRight ) {
    m_uvAtBottomLeft = uvAtBottomLeft;
    m_uvAtTopRight = uvAtTopRight;
}


void SpriteDef::GetUVs( Vec2& out_uvAtBottomLeft, Vec2& out_uvAtTopRight ) const {
    out_uvAtBottomLeft = m_uvAtBottomLeft;
    out_uvAtTopRight = m_uvAtTopRight;
}
