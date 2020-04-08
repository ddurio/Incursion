#include "Game/Tile.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"

/*
Tile::Tile( bool isStone )
    : m_isStone(isStone) {
}
*/


Tile::Tile( TileType tileType )
    : m_tileType( tileType ) {
}


void Tile::Startup() {

}


void Tile::Shutdown() {

}


void Tile::Update( float deltaSeconds ) {
    UNUSED( deltaSeconds );
}


void Tile::Render() const {
    g_theRenderer->DrawVertexArray( m_tileVerts );
}


const AABB2& Tile::GetBounds() const {
    return m_tileBounds;
}


void Tile::SetIndexAndCoords( int tileIndex, IntVec2 tileCoords ) {
    m_tileIndex = tileIndex;
    m_tileCoords = tileCoords;

    Vec2 mins = Vec2( (float)m_tileCoords.x, (float)m_tileCoords.y );
    Vec2 maxs = mins + Vec2( 1.f, 1.f );
    m_tileBounds = AABB2( mins, maxs );

    SetTileVerts();
}


void Tile::SetTileType( TileType tileType ) {
    m_tileType = tileType;
    SetTileVerts();
}

/*
void Tile::SetIsStone( bool isStone ) {
    if( m_isStone != isStone ) {
        m_isStone = isStone;
        SetTileVerts();
    }
}
*/


bool Tile::IsSolid() const {
    return TileDef::GetTileDef( m_tileType ).IsSolid();
}


float Tile::GetMovementModifier() const {
    return TileDef::GetTileDef( m_tileType ).GetMovementModifier();
}


const TileType Tile::GetTileType() const {
    return m_tileType;
}


void Tile::SetTileVerts() {
    m_tileVerts.clear();
    m_tileVerts.shrink_to_fit();

    TileDef tileDef = TileDef::GetTileDef( m_tileType );

    Rgba color = tileDef.GetTint();

    Vec2 uvMins;
    Vec2 uvMaxs;
    tileDef.GetUVs( uvMins, uvMaxs );

    AddVertsForAABB2D( m_tileVerts, m_tileBounds, color, uvMins, uvMaxs );
}
