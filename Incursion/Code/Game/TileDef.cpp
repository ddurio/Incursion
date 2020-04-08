#include "Game/TileDef.hpp"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include "Engine/Math/IntVec2.hpp"

#include "Game/GameCommon.hpp"


TileDef* TileDef::s_tileDefs[NUM_TILE_TYPES] = {};
Texture* TileDef::s_terrainTexture = nullptr;
SpriteSheet* TileDef::s_terrainSprites = nullptr;

TileDef::TileDef( Rgba tint, Vec2 uvAtMins, Vec2 uvAtMaxs, bool isSolid, float movementModifier /*= 1.f*/ ) :
    m_tint(tint),
    m_uvAtMins(uvAtMins),
    m_uvAtMaxs(uvAtMaxs),
    m_isSolid(isSolid),
    m_movementModifier(movementModifier) {
}


bool TileDef::IsSolid() const {
    return m_isSolid;
}


float TileDef::GetMovementModifier() const {
    return m_movementModifier;
}


const Rgba& TileDef::GetTint() const {
    return m_tint;
}


void TileDef::GetUVs( Vec2& out_uvMins, Vec2& out_uvMaxs ) const {
    out_uvMins = m_uvAtMins;
    out_uvMaxs = m_uvAtMaxs;
}


void TileDef::InitializeTileDefs() {
    s_terrainTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_MAP_TERRAIN );
    s_terrainSprites = new SpriteSheet( s_terrainTexture, IntVec2( 8, 8 ) );

    Vec2 uvMins;
    Vec2 uvMaxs;

    s_terrainSprites->GetSpriteDef( 3 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_GRASS] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false );

    s_terrainSprites->GetSpriteDef( 18 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_SAND] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false );

    s_terrainSprites->GetSpriteDef( 20 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_DIRT] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false );

    s_terrainSprites->GetSpriteDef( 21 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_MUD] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false, 0.5f );

    s_terrainSprites->GetSpriteDef( 26 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_STONE] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, true );

    s_terrainSprites->GetSpriteDef( 30 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_GRAVEL] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false );

    s_terrainSprites->GetSpriteDef( 36 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_STONE_BRICK] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, true );

    s_terrainSprites->GetSpriteDef( 40 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_BRICK] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, true );

    s_terrainSprites->GetSpriteDef( 54 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_ICE] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false, 1.5f );

    s_terrainSprites->GetSpriteDef( 57 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_EXIT] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, false );

    s_terrainSprites->GetSpriteDef( 58 ).GetUVs( uvMins, uvMaxs );
    s_tileDefs[TILE_TYPE_WOOD_BRICK] = new TileDef( Rgba::WHITE, uvMins, uvMaxs, true );
}


void TileDef::DestroyTileDefs() {
    for( int tileIndex = 0; tileIndex < NUM_TILE_TYPES; tileIndex++ ) {
        delete s_tileDefs[tileIndex];
        s_tileDefs[tileIndex] = nullptr;
    }
}


const TileDef& TileDef::GetTileDef( TileType tileType ) {
    return *s_tileDefs[tileType];
}


const SpriteSheet& TileDef::GetSpriteSheet() {
    return *s_terrainSprites;
}
