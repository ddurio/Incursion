#pragma once
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"

#include "vector"

class Explosion;
class PlayerTank;
struct RaycastResult;
class SpriteSheet;

class Map {
    public:
    //Map();
    Map( const IntVec2& dimensions, TileType groundType, TileType wallType, std::map<TileType, float> randomTileFractionsByType, std::map<EntityType, int> numEntitiesByType, bool arenaMode );
    ~Map() {};

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render() const;

    bool HandleKeyPressed( unsigned char keyCode );
    //bool HandleKeyReleased( unsigned char keyCode );

    const IntVec2& GetDimensions() const;
    const IntVec2 GetTileCoordsFromWorldCoords( const Vec2& worldCoords ) const;
    int GetTileIndexFromTileCoords( const IntVec2& tileCoords ) const;
    int GetTileIndexFromTileCoords( int xIndex, int yIndex ) const;
    int GetTileIndexFromWorldCoords( const Vec2& worldCoords ) const;
    const Tile& GetTileFromTileCoords( const IntVec2& tileCoords ) const;
    const Tile& GetTileFromTileCoords( int xIndex, int yIndex ) const;
    const Tile& GetTileFromWorldCoords( const Vec2& worldCoords ) const;
    PlayerTank* GetPlayer( int playerIndex ) const;
    bool AreAllPlayersDead() const;
    bool IsOnlyOnePlayerAlive() const;
    bool IsArenaMode() const;

    Entity* SpawnNewEntity( EntityType type, const Vec2& position, float orientationDegrees, Entity* source = nullptr );
    Explosion* SpawnNewExplosion( const Vec2& position, float scale, float duration = EXPLOSION_DURATION );

    void AddEntityToMap( Entity& entity );
    void AddEntityToList( Entity& entity, EntityList& list );

    void RemoveEntityFromMap( Entity& entity );
    void RemoveEntityFromList( Entity& entity, EntityList& list );

    const RaycastResult Raycast( const Vec2& startPosition, const Vec2& normalizedDirection, float maxDistance = MAP_RAYCAST_MAX_DISTANCE ) const;
    bool HasLineOfSight( const Entity* source, const Entity* destination ) const;
    Entity* AcquireNewTarget() const;

    void SendPlayersToNewMap( Map* newMap );

    static void PushEntitiesOutOfEachOther( Entity* entity1, Entity* entity2 );

    private:
    const IntVec2 m_mapDimensions = IntVec2( MAP_WIDTH, MAP_HEIGHT );
    const TileType m_groundType = TILE_TYPE_GRASS;
    const TileType m_wallType = TILE_TYPE_STONE;
    std::map<TileType, float> m_tileFractions = {
        { TILE_TYPE_STONE, 0.2f },
        { TILE_TYPE_MUD, 0.0f }
    };
    std::map<EntityType, int> m_numEntities = {
        { ENTITY_TYPE_ENEMYTANK, 1 },
        { ENTITY_TYPE_ENEMYTURRET, 1 },
        { ENTITY_TYPE_BOULDER, 1 },
    };
    const bool m_arenaMode = false;

    std::vector<Tile> m_tiles = {};

    EntityList m_entities = {};
    EntityList m_entitiesByType[NUM_ENTITY_TYPES] = {};
    //EntityList m_entitiesByFactions[NUM_FACTIONS] = {};

    EntityList m_explosions = {};

    void StartupMakeAllGroundTiles();
    void StartupAddWallBorder();
    void StartupAddRandomTiles( TileType type, float fraction );
    void StartupAddSafeBunkers();
    void StartupAddEntities( EntityType type, int numEnemies );

    std::vector<Vertex_PCU> BuildMapVerts() const;

    void UpdateFromController( float deltaSeconds );
    void UpdateCollision();
    void CollectGarbage();
    void DestroyEntity( Entity& entity );
};