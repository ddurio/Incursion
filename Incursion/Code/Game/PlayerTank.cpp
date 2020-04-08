#include "Game/PlayerTank.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"
#include "Game/Map.hpp"

PlayerTank::PlayerTank( Map* map, int playerID ) :
    Entity( ENTITY_TYPE_PLAYERTANK, (map->IsArenaMode() ? (FactionID)playerID : (FactionID)0) ),
    m_playerID(playerID) {
    m_map = map;
}


void PlayerTank::Die() {
    SoundID deathSound = g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_DIE );
    g_theAudio->PlaySound( deathSound );

    Vec2 offset = Vec2::MakeFromPolarDegrees( 0.f, 0.25f );
    m_map->SpawnNewExplosion( m_position + offset, EXPLOSION_SCALE_LARGE );

    offset = Vec2::MakeFromPolarDegrees( 120.f, 0.25f );
    m_map->SpawnNewExplosion( m_position + offset, EXPLOSION_SCALE_LARGE );

    offset = Vec2::MakeFromPolarDegrees( 240.f, 0.25f );
    m_map->SpawnNewExplosion( m_position + offset, EXPLOSION_SCALE_LARGE );

    m_isDead = true;
}


void PlayerTank::Startup() {
    SetStartPosition();
    m_velocity = Vec2( 0, 0 );
    m_orientationDegrees = 0.f;
    m_health = PLAYERTANK_MAX_HEALTH;

    m_physicsRadius = PLAYERTANK_PHYSICS_RADIUS;
    m_cosmeticRadius = PLAYERTANK_COSMETIC_RADIUS;

    StartupTexture();

    m_tankVertOffsets = AABB2( Vec2( -PLAYERTANK_COSMETIC_BOX_OFFSET, -PLAYERTANK_COSMETIC_BOX_OFFSET ), Vec2( PLAYERTANK_COSMETIC_BOX_OFFSET, PLAYERTANK_COSMETIC_BOX_OFFSET ) );

    for( int lifeIndex = 0; lifeIndex < PLAYERTANK_EXTRA_LIVES; lifeIndex++ ) {
        PlayerTank* life = new PlayerTank( m_map, m_playerID );
        m_extraLives[lifeIndex] = life;
        life->StartupExtraLife( lifeIndex );
    }

    UpdateTankVerts();
}


void PlayerTank::StartupExtraLife( int lifeIndex ) {
    m_extraLifeIndex = lifeIndex;

    // Leave cosmetic radius large to facilitate the lives being spaced out on the screen
    m_cosmeticRadius = PLAYERTANK_COSMETIC_RADIUS;

    m_velocity = Vec2( 0.f, 0.f );
    m_orientationDegrees = 0.f;

    StartupTexture();

    m_tankVertOffsets = AABB2( Vec2( -PLAYERTANK_COSMETIC_BOX_OFFSET, -PLAYERTANK_COSMETIC_BOX_OFFSET ), Vec2( PLAYERTANK_COSMETIC_BOX_OFFSET, PLAYERTANK_COSMETIC_BOX_OFFSET ) );

    m_scale = .5f;

    UpdateExtraLife( 0.f );
}


void PlayerTank::Shutdown() {
    m_tankBaseVerts.clear();
    m_tankTopVerts.clear();
}


void PlayerTank::Update( float deltaSeconds ) {
    UpdateFromController( deltaSeconds );
    m_isSolid = g_theGame->IsPlayerTankCollisionEnabled();

    // Update Extra Lives
    for( int lifeIndex = 0; lifeIndex < PLAYERTANK_EXTRA_LIVES; lifeIndex++ ) {
        if( m_extraLives[lifeIndex] != nullptr ) {
            m_extraLives[lifeIndex]->UpdateExtraLife( deltaSeconds );
        }
    }

    // Catch Game Ending or Dead
    bool gameHasBeenBeaten = g_theGame->HasGameBeenBeaten();

    if( m_isDead || gameHasBeenBeaten ) {
        if( !HasLivesRemaing() || gameHasBeenBeaten ) {
            m_deathCountdown -= deltaSeconds;

            if( m_deathCountdown <= 0.f ) {
                g_theGame->EndGame();
            }
        }

        if( m_isDead ) {
            return;
        }
    }

    // Update Turret
    float maxDeltaDegrees = PLAYERTANK_TURN_SPEED * deltaSeconds;
    m_orientationDegrees = GetTurnedTowards( m_orientationDegrees, m_desiredOrientationBaseDegrees, maxDeltaDegrees );

    // Update Position
    float thrustSpeed = 0.f;
    if( m_isThrusting ) {
        thrustSpeed = PLAYERTANK_MAX_SPEED * deltaSeconds;
    } else if( m_thrustFraction > 0.f ) {
        thrustSpeed = PLAYERTANK_MAX_SPEED * m_thrustFraction * deltaSeconds;
    }

    float tileMovementModifier = m_map->GetTileFromWorldCoords( m_position ).GetMovementModifier();

    m_position += thrustSpeed * tileMovementModifier * GetForwardVector();

    maxDeltaDegrees = PLAYERTANK_TOP_TURN_SPEED * deltaSeconds;
    m_orientationTopDegrees = GetTurnedTowards( m_orientationTopDegrees, m_desiredOrientationTopDegrees, maxDeltaDegrees );

    UpdateTankVerts();

    // Check for Exit Tile
    const Tile currentTile = m_map->GetTileFromWorldCoords( m_position );
    if( currentTile.GetTileType() == TILE_TYPE_EXIT ) {
        if( m_map->IsArenaMode() && !m_map->IsOnlyOnePlayerAlive() ) {
            return;
        }

        // EXIT LEVEL
        g_theGame->StartNextMap();
    }
}


void PlayerTank::UpdateExtraLife( float deltaSeconds ) {
    UNUSED( deltaSeconds );

    const Camera activeCamera = g_theGame->GetActiveCamera();
    Vec2 mins = activeCamera.GetOrthoBottomLeft();
    Vec2 maxs = activeCamera.GetOrthoTopRight();

    float positionX = (PLAYERTANK_EXTRA_LIVES - m_extraLifeIndex) * m_cosmeticRadius;
    positionX = (m_playerID == 0 || m_playerID == 2) ? mins.x + positionX : maxs.x - positionX; // Left vs Right of screen

    float positionY = (m_playerID == 0 || m_playerID == 1) ? maxs.y - m_cosmeticRadius : mins.y + m_cosmeticRadius; // Top vs Bottom of screen

    m_position = Vec2( positionX, positionY );

    UpdateTankVerts();
}


void PlayerTank::Render() const {
    // Render Extra Lives (even when dead)
    for( int lifeIndex = 0; lifeIndex < PLAYERTANK_EXTRA_LIVES; lifeIndex++ ) {
        if( m_extraLives[lifeIndex] != nullptr ) {
            m_extraLives[lifeIndex]->Render();
        }
    }

    if( !m_isDead ) {
        if( g_theGame->IsDebugDrawingOn() ) {
            g_theRenderer->BindTexture( nullptr );
            g_theRenderer->DrawVertexArray( m_debugCosmeticVerts );
        }

        g_theRenderer->BindTexture( m_baseTexture );
        g_theRenderer->DrawVertexArray( m_tankBaseVerts );

        g_theRenderer->BindTexture( m_topTexture );
        g_theRenderer->DrawVertexArray( m_tankTopVerts );

        if( g_theGame->IsDebugDrawingOn() ) {
            g_theRenderer->BindTexture( nullptr );
            g_theRenderer->DrawVertexArray( (int)m_debugPhysicsVerts.size(), m_debugPhysicsVerts.data() );
        }
    }
}


/*
bool PlayerTank::HandleKeyPressed( unsigned char keyCode ) {
    switch( keyCode ) {
        case( 0x20 ): { // Space
            ShootGun();
            return 0;
        }
    }

    return 1;
}
*/


int PlayerTank::GetPlayerID() const {
    return m_playerID;
}


bool PlayerTank::HasLivesRemaing() const {
    for( int lifeIndex = 0; lifeIndex < PLAYERTANK_EXTRA_LIVES; lifeIndex++ ) {
        if( m_extraLives[lifeIndex] != nullptr ) {
            return true;
        }
    }

    return false;
}


void PlayerTank::OnCollisionEntity( Entity* collidingEntity ) {
    if( collidingEntity->IsSolid() && m_isSolid ) {
        if( collidingEntity->IsMovable() ) {
            m_map->PushEntitiesOutOfEachOther( this, collidingEntity );
        } else {
            Vec2 collidingPosition;
            float collidingPhysicsRadius;

            collidingEntity->GetPhysicsDisc( collidingPosition, collidingPhysicsRadius );
            PushDiscOutOfDisc( m_position, m_physicsRadius, collidingPosition, collidingPhysicsRadius );
        }
    }
}


void PlayerTank::OnCollisionTile( Tile* collidingTile ) {
    if( collidingTile->IsSolid() && m_isSolid ) {
        PushDiscOutOfAABB2( m_position, m_physicsRadius, collidingTile->GetBounds() );
        UpdateTankVerts();
    }
}


void PlayerTank::SetStartPosition() {
    IntVec2 mapDimensions = m_map->GetDimensions();
    float offsetX = m_map->IsArenaMode() ? mapDimensions.x - 3.f : 1.f;
    float offsetY = m_map->IsArenaMode() ? mapDimensions.y - 3.f : 1.f;

    float positionX = (m_playerID % 2) * offsetX + 1.5f;
    float positionY = (1 - (m_playerID / 2)) * offsetY + 1.5f;
    m_position = Vec2( positionX, positionY );

    m_orientationDegrees = 0;
    m_orientationTopDegrees = 0;
}


void PlayerTank::SetInvincible( bool isInvincible ) {
    m_isKillable = !isInvincible;
}


void PlayerTank::StartupTexture() {
    if( m_playerID == 0 ) {
        m_factionTint = Rgba( 0.6f, 0.6f, 1.f, 1.f );
        m_baseTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE0 );
        m_topTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP0 );
    } else if( m_playerID == 1 ) {
        m_baseTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE1 );
        m_topTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP1 );
    } else if( m_playerID == 2 ) {
        m_baseTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE2 );
        m_topTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP2 );
    } else {
        m_baseTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE3 );
        m_topTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP3 );
    }
}


void PlayerTank::UpdateFromController( float deltaSeconds ) {
    if( m_playerID < 0 ) {
        return;
    }

    const XboxController& controller = g_theInput->GetController( m_playerID );

    if( m_isDead ) {
        const KeyButtonState yButton = controller.GetKeyButtonState( XBOX_BUTTON_ID_Y );

        if( yButton.WasJustPressed() ) {
            Respawn();
        } else {
            return;
        }
    }

    // PlayerTank Movement
    const AnalogJoystick& leftStick = controller.GetLeftJoystick();
    float leftMagnitude = leftStick.GetMagnitude();
    m_thrustFraction = leftMagnitude;

    if( leftMagnitude > 0.f ) {
        m_desiredOrientationBaseDegrees = leftStick.GetAngleDegrees();
    } else {
        m_desiredOrientationBaseDegrees = m_orientationDegrees;
    }

    // PlayerTank Top Movement
    const AnalogJoystick& rightStick = controller.GetRightJoystick();
    float rightMagnitude = rightStick.GetMagnitude();

    if( rightMagnitude > 0.f ) {
        m_desiredOrientationTopDegrees = rightStick.GetAngleDegrees();
    } else {
        m_desiredOrientationTopDegrees = m_orientationTopDegrees;
    }

    // Gun Cooldown and Shoot
    m_gunCooldown -= deltaSeconds;

    const KeyButtonState& aButton = controller.GetKeyButtonState( XBOX_BUTTON_ID_A );
    const KeyButtonState& rTrigger = controller.GetKeyButtonState( XBOX_BUTTON_ID_RTRIGGER );

    if( aButton.IsPressed() || rTrigger.IsPressed() ) {
        ShootGun();
    }
}


void PlayerTank::UpdateTankVerts() {
    m_tankBaseVerts.clear();
    m_tankTopVerts.clear();

    // Base Verts
    AddVertsForAABB2D( m_tankBaseVerts, m_tankVertOffsets, m_factionTint, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_tankBaseVerts, m_scale, m_orientationDegrees, m_position );

    // Top Verts
    AddVertsForAABB2D( m_tankTopVerts, m_tankVertOffsets, m_factionTint, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_tankTopVerts, m_scale, m_orientationTopDegrees, m_position );

    if( g_theGame->IsDebugDrawingOn() ) {
        UpdateDebugVerts();
    }
}


const Vec2 PlayerTank::GetForwardVectorTop() const {
    return Vec2( CosDegrees( m_orientationTopDegrees ), SinDegrees( m_orientationTopDegrees ) );
}


void PlayerTank::ShootGun() {
    if( m_gunCooldown < 0 ) {
        SoundID shootSound = g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_SHOOT );
        g_theAudio->PlaySound( shootSound );
        Vec2 barrelPosition = TransformPosition( GetForwardVectorTop(), 0.4f, 0.f, m_position );
        m_map->SpawnNewEntity( ENTITY_TYPE_BULLET, barrelPosition, m_orientationTopDegrees, (Entity*)this );
        m_map->SpawnNewExplosion( barrelPosition, EXPLOSION_SCALE_SMALL, 0.25f );
        m_gunCooldown = PLAYERTANK_RATE_OF_FIRE_SECONDS;
    }
}


void PlayerTank::Respawn() {
    if( HasLivesRemaing() ) {
        UseExtraLife();
        SetStartPosition();
        m_velocity = Vec2( 0, 0 );
        m_isDead = false;
        m_health = PLAYERTANK_MAX_HEALTH;
    }
}


void PlayerTank::UseExtraLife() {
    for( int lifeIndex = 0; lifeIndex < PLAYERTANK_EXTRA_LIVES; lifeIndex++ ) {
        if( m_extraLives[lifeIndex] != nullptr ) {
            m_extraLives[lifeIndex]->Shutdown();
            delete m_extraLives[lifeIndex];
            m_extraLives[lifeIndex] = nullptr;
            return;
        }
    }

    ERROR_RECOVERABLE( "Attempted to use extra life that doesn't exist" );
}
