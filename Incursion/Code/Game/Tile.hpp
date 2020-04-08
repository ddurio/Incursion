#pragma once
#include "vector"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileDef.hpp"


class Tile {
    friend class Map;

    public:
    //explicit Tile( bool isStone );
    explicit Tile( TileType tileType );

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render() const;

    const AABB2& GetBounds() const;
    bool IsSolid() const;
    float GetMovementModifier() const;
    const TileType GetTileType() const;

    void SetIndexAndCoords( int tileIndex, IntVec2 tileCoords );
    //void SetIsStone( bool isStone );
    void SetTileType( TileType tileType );


    private:
    int m_tileIndex = -1;
    IntVec2 m_tileCoords = IntVec2( -1, -1 );
    //AABB2 m_tileBounds = AABB2( Vec2( -1.f, -1.f ), Vec2( -1.f, -1.f ) );
    AABB2 m_tileBounds;
    std::vector<Vertex_PCU> m_tileVerts;

    TileType m_tileType = TILE_TYPE_UNKNOWN;
    //bool m_isStone = false;
    //Rgba grassColor = Rgba( 0.f, 1.f, 0.f, 1.f );
    //Rgba stoneColor = Rgba( 0.5f, 0.6f, 0.7f, 1.f );

    void SetTileVerts();
};