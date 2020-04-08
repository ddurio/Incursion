#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"


class Texture;
class SpriteSheet;

enum TileType {
    TILE_TYPE_UNKNOWN = -1,
    TILE_TYPE_GRASS,
    TILE_TYPE_SAND,
    TILE_TYPE_DIRT,
    TILE_TYPE_MUD,
    TILE_TYPE_STONE,
    TILE_TYPE_GRAVEL,
    TILE_TYPE_STONE_BRICK,
    TILE_TYPE_BRICK,
    TILE_TYPE_ICE,
    TILE_TYPE_EXIT,
    TILE_TYPE_WOOD_BRICK,
    //TILE_TYPE_WATER,
    //TILE_TYPE_LAVA,
    NUM_TILE_TYPES
};

class TileDef {
    public:
    explicit TileDef( Rgba tint, Vec2 uvAtMins, Vec2 uvAtMaxs, bool m_isSolid, float movementModifier = 1.f );

    bool IsSolid() const;
    float GetMovementModifier() const;
    const Rgba& GetTint() const;
    void GetUVs( Vec2& out_uvMins, Vec2& out_uvMaxs ) const;

    static void InitializeTileDefs();
    static void DestroyTileDefs();

    static const TileDef& GetTileDef( TileType tileType );
    static const SpriteSheet& GetSpriteSheet();

    private:
    bool m_isSolid = false;
    float m_movementModifier = 1.0f;
    Rgba m_tint = Rgba::WHITE;
    Vec2 m_uvAtMins = Vec2::ZERO;
    Vec2 m_uvAtMaxs = Vec2::ONE;

    static TileDef* s_tileDefs[NUM_TILE_TYPES];
    static Texture* s_terrainTexture;
    static SpriteSheet* s_terrainSprites;
};