#include "Game/Map.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDef.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"

#include "Game/Boulder.hpp"
#include "Game/Bullet.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/EnemyTurret.hpp"
#include "Game/Explosion.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/RaycastResult.hpp"


Map::Map( const IntVec2& dimensions, TileType groundType, TileType wallType, std::map<TileType, float> randomTileFractionsByType, std::map<EntityType, int> numEntitiesByType, bool arenaMode ) :
    m_mapDimensions( dimensions ),
    m_groundType( groundType ),
    m_wallType( wallType ),
    m_tileFractions( randomTileFractionsByType ),
    m_numEntities( numEntitiesByType ),
    m_arenaMode( arenaMode ) {

    m_entitiesByType[ENTITY_TYPE_PLAYERTANK].clear();
    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        m_entitiesByType[ENTITY_TYPE_PLAYERTANK].push_back( nullptr );
    }
}


void Map::Startup() {
    UpdateFromController( 0.f );

    StartupMakeAllGroundTiles(); // Initialize ground
    StartupAddWallBorder(); // Add border

    // Add all random tiles as requested
    std::map<TileType, float>::iterator tileIter;
    for( tileIter = m_tileFractions.begin(); tileIter != m_tileFractions.end(); tileIter++ ) {
        TileType type = tileIter->first;
        float fraction = tileIter->second;

        StartupAddRandomTiles( type, fraction );
    }

    StartupAddSafeBunkers(); // Add safe bunker at starting (and eventually ending) point

    // Add all entities as requested
    std::map<EntityType, int>::iterator entityIter;
    for( entityIter = m_numEntities.begin(); entityIter != m_numEntities.end(); entityIter++ ) {
        EntityType type = entityIter->first;
        int numEntities = entityIter->second;

        StartupAddEntities( type, numEntities );
    }
}


void Map::Shutdown() {
    int numEntities = (int)m_entities.size();
    for( int entityIndex = 0; entityIndex < numEntities; entityIndex++ ) {
        Entity* entity = m_entities[entityIndex];
        if( entity != nullptr ) {
            DestroyEntity( *entity );
        }
    }

    int numExplosions = (int)m_explosions.size();
    for( int explosionIndex = 0; explosionIndex < numExplosions; explosionIndex++ ) {
        Entity* explosion = m_explosions[explosionIndex];
        if( explosion != nullptr ) {
            DestroyEntity( *explosion );
        }
    }

    m_entities.clear();
    m_explosions.clear();
    m_tiles.clear();

    for( int typeIndex = 0; typeIndex < NUM_ENTITY_TYPES; typeIndex++ ) {
        m_entitiesByType[typeIndex].clear();
    }
}


void Map::Update( float deltaSeconds ) {
    UpdateFromController( deltaSeconds );

    for( int entityIndex = 0; entityIndex < (int)m_entities.size(); entityIndex++ ) {
        Entity* entity = m_entities[entityIndex];
        if( entity != nullptr ) {
            entity->Update( deltaSeconds );
        }
    }

    for( int explosionIndex = 0; explosionIndex < (int)m_explosions.size(); explosionIndex++ ) {
        Entity* explosion = m_explosions[explosionIndex];
        if( explosion != nullptr ) {
            explosion->Update( deltaSeconds );
        }
    }

    UpdateCollision();

    CollectGarbage();
}


void Map::Render() const {
    SpriteSheet terrainSprites = TileDef::GetSpriteSheet();
    const Texture* terrainTexture = terrainSprites.GetTexture();
    g_theRenderer->BindTexture( terrainTexture );

    std::vector<Vertex_PCU> mapVerts;
    mapVerts = BuildMapVerts();
    g_theRenderer->DrawVertexArray( mapVerts );

    int numEntities = (int)m_entities.size();

    for( int entityIndex = 0; entityIndex < numEntities; entityIndex++ ) {
        Entity* entity = m_entities[entityIndex];
        if( entity != nullptr ) {
            entity->Render();
        }
    }

    int numExplosions = (int)m_explosions.size();

    for( int explosionIndex = 0; explosionIndex < numExplosions; explosionIndex++ ) {
        Entity* explosion = m_explosions[explosionIndex];
        if( explosion != nullptr ) {
            explosion->Render();
        }
    }
}


bool Map::HandleKeyPressed( unsigned char keyCode ) {
    UNUSED( keyCode );
    // Not using keyboard controls for PlayerTanks
    return 1;
}


const IntVec2& Map::GetDimensions() const {
    return m_mapDimensions;
}


const IntVec2 Map::GetTileCoordsFromWorldCoords( const Vec2& worldCoords ) const {
    int tempX = ClampInt( (int)worldCoords.x, 0, m_mapDimensions.x - 1);
    int tempY = ClampInt( (int)worldCoords.y, 0, m_mapDimensions.y - 1);
    return IntVec2( tempX, tempY );
}


int Map::GetTileIndexFromTileCoords( const IntVec2& tileCoords ) const {
    int numTiles = (int)m_tiles.size();
    int tileIndex = (tileCoords.y * m_mapDimensions.x) + tileCoords.x;
    return ClampInt( tileIndex, 0, numTiles - 1 );
}


int Map::GetTileIndexFromTileCoords( int xIndex, int yIndex ) const {
    return GetTileIndexFromTileCoords( IntVec2( xIndex, yIndex ) );
}


int Map::GetTileIndexFromWorldCoords( const Vec2& worldCoords ) const {
    IntVec2 tileCoords = GetTileCoordsFromWorldCoords( worldCoords );
    return GetTileIndexFromTileCoords( tileCoords );
}


const Tile& Map::GetTileFromTileCoords( const IntVec2& tileCoords ) const {
    int tileIndex = GetTileIndexFromTileCoords( tileCoords );
    return m_tiles[tileIndex];
}


const Tile& Map::GetTileFromTileCoords( int xIndex, int yIndex ) const {
    int tileIndex = GetTileIndexFromTileCoords( xIndex, yIndex );
    return m_tiles[tileIndex];
}


const Tile& Map::GetTileFromWorldCoords( const Vec2& worldCoords ) const {
    IntVec2 tileCoords = GetTileCoordsFromWorldCoords( worldCoords );
    return GetTileFromTileCoords( tileCoords );
}


PlayerTank* Map::GetPlayer( int playerIndex ) const {
    return (PlayerTank*)m_entitiesByType[ENTITY_TYPE_PLAYERTANK][playerIndex];
}


bool Map::AreAllPlayersDead() const {
    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        PlayerTank* player = GetPlayer( playerIndex );

        if( player != nullptr && (player->IsAlive() || player->HasLivesRemaing()) ) {
            return false;
        }
    }

    return true;
}


bool Map::IsOnlyOnePlayerAlive() const {
    int livingPlayers = 0;

    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        PlayerTank* player = GetPlayer( playerIndex );

        if( player != nullptr && (player->IsAlive() || player->HasLivesRemaing()) ) {
            livingPlayers++;
        }
    }

    return (livingPlayers == 1);
}


bool Map::IsArenaMode() const {
    return m_arenaMode;
}


Entity* Map::SpawnNewEntity( EntityType type, const Vec2& position, float orientationDegrees, Entity* source ) {
    Entity* entity = nullptr;

    switch(type) {
        case(ENTITY_TYPE_BOULDER): {
            entity = (Entity*)(new Boulder( this ));
            break;
        } case(ENTITY_TYPE_BULLET): {
            entity = (Entity*)(new Bullet( this, source ));
            break;
        } case(ENTITY_TYPE_ENEMYTANK): {
            entity = (Entity*)(new EnemyTank( this ));
            break;
        } case(ENTITY_TYPE_ENEMYTURRET): {
            entity = (Entity*)(new EnemyTurret( this ));
            break;
        } default: {
            return nullptr;
        }
    }

    entity->m_position = position;
    entity->m_orientationDegrees = orientationDegrees;

    AddEntityToMap( *entity );
    entity->Startup();

    return entity;
}


Explosion* Map::SpawnNewExplosion( const Vec2& position, float scale, float duration /*= EXPLOSION_DURATION */ ) {
    Explosion* explosion = new Explosion( this, position, scale, duration );
    //explosion->m_orientationDegrees = g_RNG->GetRandomFloatInRange( 0.f, 360.f );

    AddEntityToMap( *(Entity*)explosion );
    explosion->Startup();

    return explosion;
}


const RaycastResult Map::Raycast( const Vec2& startPosition, const Vec2& normalizedDirection, float maxDistance /*= MAP_RAYCAST_MAX_DISTANCE*/ ) const {
    float stepDistance = maxDistance / (float)MAP_RAYCAST_NUM_SAMPLES;
    Vec2 stepStride = stepDistance * normalizedDirection;
    Vec2 samplePosition = startPosition;
    
    for( float distance = 0.f; distance < maxDistance; distance += stepDistance ) {
        const Tile tile = GetTileFromWorldCoords( samplePosition );

        if( tile.IsSolid() ) {
            RaycastResult result = RaycastResult();
            result.impactDistance = distance;
            result.impactFraction = result.impactDistance / maxDistance;
            result.impactPosition = samplePosition;
            result.impactTile = &tile;

            return result;
        }

        samplePosition += stepStride;
    }

    return RaycastResult();
}


bool Map::HasLineOfSight( const Entity* source, const Entity* destination ) const {
    Vec2 start = source->GetPosition();
    Vec2 direction = destination->GetPosition() - start;
    float maxDistance = direction.NormalizeGetPreviousLength();

    RaycastResult result = Raycast( start, direction, maxDistance );

    return !result.DidImpact();
}


Entity* Map::AcquireNewTarget() const {
    std::vector<PlayerTank*> potentialTargets;

    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        PlayerTank* player = GetPlayer( playerIndex );
        if( player != nullptr && player->IsAlive() ) {
            potentialTargets.push_back( player );
        }
    }

    int numTargets = (int)potentialTargets.size();
    if( numTargets == 0 ) {
        return nullptr;
    }

    int targetIndex = g_RNG->GetRandomIntLessThan( numTargets );
    return (Entity*)potentialTargets[targetIndex];
}


void Map::SendPlayersToNewMap( Map* newMap ) {
    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        PlayerTank* player = GetPlayer( playerIndex );
        if( player != nullptr ) {
            RemoveEntityFromMap( *(Entity*)player );
            newMap->AddEntityToMap( *(Entity*)player );

            player->m_map = newMap;
            player->SetStartPosition();

            if( newMap->IsArenaMode() ) {
                player->SetFaction( (FactionID)player->GetPlayerID() );
            } else {
                player->SetFaction( (FactionID)0 );
            }
        }
    }

}


void Map::AddEntityToMap( Entity& entity ) {
    // Everyone added to full entity list
    AddEntityToList( entity, m_entities );

    EntityType type = entity.GetEntityType();

    // Explosions in their own list, everyone else in the list by type
    if( type == ENTITY_TYPE_EXPLOSION ) {
        AddEntityToList( entity, m_explosions );
    } else {
        AddEntityToList( entity, m_entitiesByType[type] );
    }
}


void Map::AddEntityToList( Entity& entity, EntityList& list ) {
    int numEntities = (int)list.size();

    for( int entityIndex = 0; entityIndex < numEntities; entityIndex++ ) {
        if( list[entityIndex] == nullptr ) {
            list[entityIndex] = &entity;
            return;
        }
    }

    list.push_back( &entity );
}


void Map::RemoveEntityFromMap( Entity& entity ) {
    RemoveEntityFromList( entity, m_entities );

    EntityType type = entity.GetEntityType();

    if( type == ENTITY_TYPE_EXPLOSION ) {
        RemoveEntityFromList( entity, m_explosions );
    } else {
        RemoveEntityFromList( entity, m_entitiesByType[type] );
    }

    if( type == ENTITY_TYPE_PLAYERTANK ) {
        PlayerTank* player = (PlayerTank*)&entity;
        int playerID = player->GetPlayerID();
        m_entitiesByType[ENTITY_TYPE_PLAYERTANK][playerID] = nullptr;
    }
}


void Map::RemoveEntityFromList( Entity& entity, EntityList& list ) {
    int numEntities = (int)list.size();

    for( int entityIndex = 0; entityIndex < numEntities; entityIndex++ ) {
        if( list[entityIndex] == &entity ) {
            list[entityIndex] = nullptr;
            return;
        }
    }
}


void Map::PushEntitiesOutOfEachOther( Entity* entity1, Entity* entity2 ) {
    Vec2& position1 = entity1->m_position;
    Vec2& position2 = entity2->m_position;

    float radius1 = entity1->m_physicsRadius;
    float radius2 = entity2->m_physicsRadius;

    PushDiscsOutOfEachOther( position1, radius1, position2, radius2 );
}


void Map::StartupMakeAllGroundTiles() {
    int numTiles = m_mapDimensions.x * m_mapDimensions.y;
    int tileCoordsX;
    int tileCoordsY;

    // Make the entire map Grass Tiles
    for( int i = 0; i < numTiles; i++ ) {
        m_tiles.push_back( Tile( m_groundType ) );
        tileCoordsX = i % m_mapDimensions.x;
        tileCoordsY = i / m_mapDimensions.x;
        m_tiles[i].SetIndexAndCoords( i, IntVec2( tileCoordsX, tileCoordsY ) );
    }
}


void Map::StartupAddWallBorder() {
    int tileIndex;

    // Make top and bottom Stone Tiles
    for( int x = 0; x < m_mapDimensions.x; x++ ) {
        // Bottom Row
        tileIndex = x;
        m_tiles[tileIndex].SetTileType( m_wallType );

        // Top Row
        tileIndex = (m_mapDimensions.y - 1) * m_mapDimensions.x + x;
        m_tiles[tileIndex].SetTileType( m_wallType );
    }

    // Make left and right Stone Tiles
    for( int y = 0; y < m_mapDimensions.y; y++ ) {
        // Left Column
        tileIndex = m_mapDimensions.x * y;
        m_tiles[tileIndex].SetTileType( m_wallType );

        // Right Column
        tileIndex = (m_mapDimensions.x * y) + (m_mapDimensions.x - 1);
        m_tiles[tileIndex].SetTileType( m_wallType );
    }
}


void Map::StartupAddRandomTiles( TileType type, float fraction ) {
    int numTiles = m_mapDimensions.x * m_mapDimensions.y;
    int tileCoordX;
    int tileCoordY;
    int tileIndex;

    for( int i = 0; i < numTiles * fraction; i++ ) {
        // Only pick tiles on the interior of the border
        tileCoordX = g_RNG->GetRandomIntInRange( 1, m_mapDimensions.x - 2 );
        tileCoordY = g_RNG->GetRandomIntInRange( 1, m_mapDimensions.y - 2 );
        tileIndex = GetTileIndexFromTileCoords( tileCoordX, tileCoordY );
        m_tiles[tileIndex].SetTileType( type );
    }
}


void Map::StartupAddSafeBunkers() {
    int tileIndex;

    // Clear starting safe zone to grass
    int xMinLeft = 1;
    int xMaxLeft = MAP_STARTING_SAFE_ZONE_SIZE_X;
    int yMinBot = 1;
    int yMaxBot = MAP_STARTING_SAFE_ZONE_SIZE_Y;

    // Clear ending 'safe' zone to mud
    int xMinRight = (m_mapDimensions.x - 1) - MAP_STARTING_SAFE_ZONE_SIZE_X;
    int xMaxRight = m_mapDimensions.x - 2;
    int yMinTop = (m_mapDimensions.y - 1) - MAP_STARTING_SAFE_ZONE_SIZE_Y;
    int yMaxTop = m_mapDimensions.y - 2;

    int xCenter = m_mapDimensions.x / 2;
    int yCenter = m_mapDimensions.y / 2;

    // BottomLeft bunker always made
    TileType type = m_arenaMode ? TILE_TYPE_MUD : m_groundType;
    for( int xIndex = xMinLeft; xIndex <= xMaxLeft; xIndex++ ) {
        for( int yIndex = yMinBot; yIndex <= yMaxBot; yIndex++ ) {
            tileIndex = GetTileIndexFromTileCoords( xIndex, yIndex );
            m_tiles[tileIndex].SetTileType( type );
        }
    }

    for( int xIndex = xMinRight; xIndex <= xMaxRight; xIndex++ ) {
        for( int yIndex = yMinTop; yIndex <= yMaxTop; yIndex++ ) {
            tileIndex = GetTileIndexFromTileCoords( xIndex, yIndex );
            m_tiles[tileIndex].SetTileType( TILE_TYPE_MUD );
        }
    }

    if( m_arenaMode ) { // Add mud for extra bunkers TL, BR, and Center
        // TL
        for( int xIndex = xMinLeft; xIndex <= xMaxLeft; xIndex++ ) {
            for( int yIndex = yMinTop; yIndex <= yMaxTop; yIndex++ ) {
                tileIndex = GetTileIndexFromTileCoords( xIndex, yIndex );
                m_tiles[tileIndex].SetTileType( TILE_TYPE_MUD );
            }
        }

        // BR
        for( int xIndex = xMinRight; xIndex <= xMaxRight; xIndex++ ) {
            for( int yIndex = yMinBot; yIndex <= yMaxBot; yIndex++ ) {
                tileIndex = GetTileIndexFromTileCoords( xIndex, yIndex );
                m_tiles[tileIndex].SetTileType( TILE_TYPE_MUD );
            }
        }

        // Center
        for( int xIndex = xCenter - 4; xIndex <= xCenter + 4; xIndex++ ) {
            for( int yIndex = yCenter - 4; yIndex <= yCenter + 4; yIndex++ ) {
                tileIndex = GetTileIndexFromTileCoords( xIndex, yIndex );
                m_tiles[tileIndex].SetTileType( TILE_TYPE_MUD );
            }
        }
    }


    IntVec2 bunkerCoords[10] = {
        // BL Bunker (Start/Player3 Start)
        IntVec2( xMaxLeft - 1, yMaxBot - 1 ),
        IntVec2( xMaxLeft - 1, yMaxBot - 2 ),
        IntVec2( xMaxLeft - 1, yMaxBot - 3 ),
        IntVec2( xMaxLeft - 2, yMaxBot - 1 ),
        IntVec2( xMaxLeft - 3, yMaxBot - 1 ),
        // TR Bunker (Exit/Player2 Start)
        IntVec2( xMinRight + 1, yMinTop + 1),
        IntVec2( xMinRight + 1, yMinTop + 2),
        IntVec2( xMinRight + 1, yMinTop + 3),
        IntVec2( xMinRight + 2, yMinTop + 1),
        IntVec2( xMinRight + 3, yMinTop + 1),
    };



    for( IntVec2 tileCoords : bunkerCoords ) {
        tileIndex = GetTileIndexFromTileCoords( tileCoords );
        m_tiles[tileIndex].SetTileType( m_wallType );
    }

    if( m_arenaMode ) {
        IntVec2 arenaBunkerCoords[46] = {
            // TL Bunker (Player1 Start)
            IntVec2( xMaxLeft - 1, yMinTop + 1 ),
            IntVec2( xMaxLeft - 1, yMinTop + 2 ),
            IntVec2( xMaxLeft - 1, yMinTop + 3 ),
            IntVec2( xMaxLeft - 2, yMinTop + 1 ),
            IntVec2( xMaxLeft - 3, yMinTop + 1 ),
            // BR Bunker (Player4 Start)
            IntVec2( xMinRight + 1, yMaxBot - 1 ),
            IntVec2( xMinRight + 1, yMaxBot - 2 ),
            IntVec2( xMinRight + 1, yMaxBot - 3 ),
            IntVec2( xMinRight + 2, yMaxBot - 1 ),
            IntVec2( xMinRight + 3, yMaxBot - 1 ),
            // Arena Exit Bunker (left to right, top to bottom)
            IntVec2( xCenter - 4, yCenter - 4 ),
            IntVec2( xCenter - 4, yCenter - 3 ),
            IntVec2( xCenter - 4, yCenter - 2 ),
            IntVec2( xCenter - 4, yCenter - 1 ),
            IntVec2( xCenter - 4, yCenter + 1 ),
            IntVec2( xCenter - 4, yCenter + 2 ),
            IntVec2( xCenter - 4, yCenter + 3 ),
            IntVec2( xCenter - 4, yCenter + 4 ),
            IntVec2( xCenter - 3, yCenter - 4 ),
            IntVec2( xCenter - 3, yCenter + 4 ),
            IntVec2( xCenter - 2, yCenter - 4 ),
            IntVec2( xCenter - 2, yCenter - 1 ),
            IntVec2( xCenter - 2, yCenter ),
            IntVec2( xCenter - 2, yCenter + 1 ),
            IntVec2( xCenter - 2, yCenter + 4 ),
            IntVec2( xCenter - 1, yCenter - 4 ),
            IntVec2( xCenter - 1, yCenter + 4 ),
            IntVec2( xCenter,     yCenter - 2 ),
            IntVec2( xCenter,     yCenter + 2 ),
            IntVec2( xCenter + 1, yCenter - 4 ),
            IntVec2( xCenter + 1, yCenter + 4 ),
            IntVec2( xCenter + 2, yCenter - 4 ),
            IntVec2( xCenter + 2, yCenter - 1 ),
            IntVec2( xCenter + 2, yCenter ),
            IntVec2( xCenter + 2, yCenter + 1 ),
            IntVec2( xCenter + 2, yCenter + 4 ),
            IntVec2( xCenter + 3, yCenter - 4 ),
            IntVec2( xCenter + 3, yCenter + 4 ),
            IntVec2( xCenter + 4, yCenter - 4 ),
            IntVec2( xCenter + 4, yCenter - 3 ),
            IntVec2( xCenter + 4, yCenter - 2 ),
            IntVec2( xCenter + 4, yCenter - 1 ),
            IntVec2( xCenter + 4, yCenter + 1 ),
            IntVec2( xCenter + 4, yCenter + 2 ),
            IntVec2( xCenter + 4, yCenter + 3 ),
            IntVec2( xCenter + 4, yCenter + 4 ),
        };

        for( IntVec2 tileCoords : arenaBunkerCoords ) {
            tileIndex = GetTileIndexFromTileCoords( tileCoords );
            m_tiles[tileIndex].SetTileType( m_wallType );
        }
    }

    if( m_arenaMode ) {
        tileIndex = GetTileIndexFromTileCoords( xCenter, yCenter );
    } else {
        tileIndex = GetTileIndexFromTileCoords( xMaxRight, yMaxTop );
    }
    m_tiles[tileIndex].SetTileType( TILE_TYPE_EXIT );
}


void Map::StartupAddEntities( EntityType type, int numEntities ) {
    for( int entityIndex = 0; entityIndex < numEntities; entityIndex++ ) {
        int xPos;
        int yPos;
        const Tile* tile;

        do { // Repeat until we get a tile that's NOT solid
            xPos = g_RNG->GetRandomIntLessThan( m_mapDimensions.x );
            yPos = g_RNG->GetRandomIntLessThan( m_mapDimensions.y );

            tile = &GetTileFromTileCoords( xPos, yPos );
        } while( tile->IsSolid() );

        Vec2 entityPos( (float)xPos + 0.5f, (float)yPos + 0.5f );
        float orientationDegrees = g_RNG->GetRandomFloatInRange( 0.f, 360.f );
        SpawnNewEntity( type, entityPos, orientationDegrees );
    }
}


std::vector<Vertex_PCU> Map::BuildMapVerts() const {
    std::vector<Vertex_PCU> mapVerts;
    // Tile Verts
    for( int i = 0; i < (int)m_tiles.size(); i++ ) {
        Tile tile = m_tiles[i];
        mapVerts.insert( mapVerts.end(), tile.m_tileVerts.begin(), tile.m_tileVerts.end() );
    }
/*
    // Vertical Grid Lines
    for( int i = 1; i < m_mapDimensions.x; i++ ) {
        AddVertsForLine2D( mapVerts, Vec2( (float)i, 0.f ), Vec2( (float)i, (float)m_mapDimensions.y ), .05f, Rgba::BLACK );
    }

    // Horizontal Grid Lines
    for( int i = 1; i < m_mapDimensions.y; i++ ) {
        AddVertsForLine2D( mapVerts, Vec2( 0.f, (float)i ), Vec2( (float)m_mapDimensions.x, (float)i ), .05f, Rgba::BLACK );
    }
*/
    return mapVerts;
}


void Map::UpdateFromController( float deltaSeconds ) {
    UNUSED( deltaSeconds );

    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        const XboxController& controller = g_theInput->GetController( playerIndex );
        
        if( controller.IsConnected() && GetPlayer(playerIndex) == nullptr ) {
            PlayerTank* player = new PlayerTank( this, playerIndex );
            m_entitiesByType[ENTITY_TYPE_PLAYERTANK][playerIndex] = player;
            m_entities.push_back( player );
            player->Startup();

            SoundID newPlayerID = g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_JOIN );
            g_theAudio->PlaySound( newPlayerID );
        }
    }
}


void Map::UpdateCollision() {
    //---------------------------------
    // Update Entity v Tile Collision
    //---------------------------------
    static const int numTileOffsets = 8;
    static const IntVec2 tileOffsets[numTileOffsets] = {
        IntVec2(  0,  1 ), // North
        IntVec2(  0, -1 ), // South
        IntVec2( -1,  0 ), // West
        IntVec2(  1,  0 ), // East
        IntVec2( -1,  1 ), // Northwest
        IntVec2(  1,  1 ), // Northeast
        IntVec2( -1, -1 ), // Southwest
        IntVec2(  1, -1 )  // Southeast
    };

    Entity* entity1 = nullptr;
    Tile* tile = nullptr;
    int numEntities = (int)m_entities.size();

    for( int entityIter = 0; entityIter < numEntities; entityIter++ ) {
        entity1 = m_entities[entityIter];

        if( entity1 != nullptr && entity1->IsAlive() && !entity1->IsGarbage() && entity1->GetEntityType() != ENTITY_TYPE_BULLET ) {
            // Bullets handle tile collision in their own update preventatively
            IntVec2 currentTileCoords = GetTileCoordsFromWorldCoords( entity1->GetPosition() );

            for( int tileIter = 0; tileIter < numTileOffsets; tileIter++ ) {
                int tileIndex = GetTileIndexFromTileCoords( currentTileCoords + tileOffsets[tileIter] );
                tile = &m_tiles[tileIndex];
                entity1->OnCollisionTile( tile );
            }
        }
    }

    //---------------------------------
    // Update Entity v Entity Collision
    //---------------------------------
    Entity* entity2 = nullptr;

    for( int entity1Index = 0; entity1Index < numEntities - 1; entity1Index++ ) {
        entity1 = m_entities[entity1Index];

        if( entity1 != nullptr && entity1->IsAlive() && !entity1->IsGarbage() ) {
            for( int entity2Index = entity1Index + 1; entity2Index < numEntities; entity2Index++ ) {
                entity2 = m_entities[entity2Index];

                if( entity2 != nullptr && entity2->IsAlive() && !entity2->IsGarbage() ) {
                    Vec2 entity1Center;
                    Vec2 entity2Center;
                    float entity1Radius;
                    float entity2Radius;

                    entity1->GetPhysicsDisc( entity1Center, entity1Radius );
                    entity2->GetPhysicsDisc( entity2Center, entity2Radius );

                    if( DoDiscsOverlap(entity1Center, entity1Radius, entity2Center, entity2Radius) ) {
                        entity1->OnCollisionEntity( entity2 );
                        entity2->OnCollisionEntity( entity1 );
                    }
                }
            }
        }
    }
}


void Map::CollectGarbage() {
    int numEntities = (int)m_entities.size();
    for( int entityIndex = 0; entityIndex < numEntities; entityIndex++ ) {
        Entity* entity = m_entities[entityIndex];
        if( entity != nullptr && entity->IsGarbage() ) {
            DestroyEntity( *entity );
        }
    }
}


void Map::DestroyEntity( Entity& entity ) {
    RemoveEntityFromMap( entity );
    entity.Shutdown();
    delete &entity;
}
