#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include "vector"


class Texture;
struct IntVec2;

class SpriteSheet {
    public:
    explicit SpriteSheet( const Texture* texture, const IntVec2& gridLayout );

    const Texture* GetTexture() const;
    const SpriteDef& GetSpriteDef( int spriteIndex ) const;
    const SpriteDef& GetSpriteDef( IntVec2 spriteCoords ) const;

    private:
    const Texture* m_texture;
    std::vector<SpriteDef> m_spriteDefs;
    int m_gridWidth = 0;
};