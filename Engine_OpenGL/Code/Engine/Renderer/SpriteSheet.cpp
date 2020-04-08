#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include "Engine/Math/Vec2.hpp"


SpriteSheet::SpriteSheet( const Texture* texture, const IntVec2& gridLayout ) {
    m_texture = texture;
    m_gridWidth = gridLayout.x;

    float uStride = 1.f / (float)gridLayout.x;
    float vStride = 1.f / (float)gridLayout.y;
    int numSprites = gridLayout.x * gridLayout.y;

    int gridX;
    int gridY;
    float minU;
    float maxU;
    float minV;
    float maxV;

    for( int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++ ) {
        gridX = spriteIndex % gridLayout.x;
        gridY = spriteIndex / gridLayout.x;

        minU = uStride * (float)gridX;
        maxU = minU + uStride;

        maxV = 1 - (vStride * (float)gridY);
        minV = maxV - vStride;

        m_spriteDefs.push_back( SpriteDef( Vec2( minU, minV ), Vec2( maxU, maxV ) ) );
    }
}

const Texture* SpriteSheet::GetTexture() const {
    return m_texture;
}

const SpriteDef& SpriteSheet::GetSpriteDef( int spriteIndex ) const {
    return m_spriteDefs[spriteIndex];
}


const SpriteDef& SpriteSheet::GetSpriteDef( IntVec2 spriteCoords ) const {
    int spriteIndex = (spriteCoords.y * m_gridWidth) + spriteCoords.x;
    return GetSpriteDef( spriteIndex );
}
