#include "Game/EnemyTurret.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/RaycastResult.hpp"


EnemyTurret::EnemyTurret( Map* map ) :
    Entity( ENTITY_TYPE_ENEMYTURRET, FACTION_ENEMY1 ) {
    m_map = map;
}


void EnemyTurret::Die() {
    SoundID deathSound = g_theAudio->CreateOrGetSound( AUDIO_ENEMY_DIE );
    g_theAudio->PlaySound( deathSound );

    m_map->SpawnNewExplosion( m_position, EXPLOSION_SCALE_LARGE );

    m_isDead = true;
    m_isGarbage = true;
}


void EnemyTurret::Startup() {
    m_velocity = Vec2( 0, 0 );
    m_orientationTopDegrees = m_orientationDegrees; // orientation set by map before calling Startup in SpawnNewEntity
    m_health = ENEMYTURRET_MAX_HEALTH;

    m_isMovable = false;

    m_physicsRadius = ENEMYTURRET_PHYSICS_RADIUS;
    m_cosmeticRadius = ENEMYTURRET_COSMETIC_RADIUS;

    m_baseTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTURRET_BASE );
    m_topTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTURRET_TOP );

    m_turretVertOffsets = AABB2( Vec2( -ENEMYTURRET_COSMETIC_BOX_OFFSET, -ENEMYTURRET_COSMETIC_BOX_OFFSET ), Vec2( ENEMYTURRET_COSMETIC_BOX_OFFSET, ENEMYTURRET_COSMETIC_BOX_OFFSET ) );

    UpdateTurretVerts();
}


void EnemyTurret::Shutdown() {
    m_isDead = true;
    m_isGarbage = true;
}


void EnemyTurret::Update( float deltaSeconds ) {
    m_gunCooldown -= deltaSeconds;

    if( m_target == nullptr || !m_target->IsAlive() ) {
        m_target = m_map->AcquireNewTarget();

        if( m_target == nullptr ) {
            m_orientationTopDegrees += ENEMYTURRET_TOP_TURN_SPEED * deltaSeconds;
            UpdateTurretVerts();
            UpdateLaserVerts();
            return;
        }
    }

    bool hasLoS = m_map->HasLineOfSight( (Entity*)this, m_target );
    Vec2 targetDisplacement = (m_target->GetPosition() - m_position);
    float targetDegrees = targetDisplacement.GetAngleDegrees();

    if( hasLoS && (targetDisplacement.GetLength() < ENEMYTURRET_MAX_SIGHT_RANGE) ) {
        m_scanForTarget = ENEMYTURRET_SCAN_TIME_SECONDS;
        m_targetLastKnownPosition = m_target->GetPosition();

        UpdateChaseTarget( deltaSeconds, targetDegrees, hasLoS );
    } else if( m_scanForTarget > 0.f ) {
        m_scanForTarget -= deltaSeconds;
        
        if( m_scanLeft ) {
            UpdateChaseTarget( deltaSeconds, targetDegrees + ENEMYTURRET_SCAN_ANGLE, hasLoS );

            if( m_orientationTopDegrees >= targetDegrees + ENEMYTURRET_SCAN_ANGLE ) {
                m_scanLeft = false;
            }
        } else {
            UpdateChaseTarget( deltaSeconds, targetDegrees - ENEMYTURRET_SCAN_ANGLE, hasLoS );

            if( m_orientationTopDegrees <= targetDegrees - ENEMYTURRET_SCAN_ANGLE ) {
                m_scanLeft = true;
            }
        }
    } else {
        m_orientationTopDegrees += ENEMYTURRET_TOP_TURN_SPEED * deltaSeconds;
    }

    UpdateTurretVerts();
    UpdateLaserVerts();

}


void EnemyTurret::Render() const {
    if( g_theGame->IsDebugDrawingOn() ) {
        g_theRenderer->BindTexture( nullptr );
        g_theRenderer->DrawVertexArray( m_debugCosmeticVerts );
    }

    g_theRenderer->BindTexture( m_baseTexture );
    g_theRenderer->DrawVertexArray( m_turretBaseVerts );

    g_theRenderer->BindTexture( nullptr );
    g_theRenderer->DrawVertexArray( m_laserVerts );

    g_theRenderer->BindTexture( m_topTexture );
    g_theRenderer->DrawVertexArray( m_turretTopVerts );

    if( g_theGame->IsDebugDrawingOn() ) {
        g_theRenderer->BindTexture( nullptr );
        g_theRenderer->DrawVertexArray( m_debugPhysicsVerts );
    }
}


void EnemyTurret::OnCollisionEntity( Entity* collidingEntity ) {
    UNUSED( collidingEntity );
    // Assume that entity will push out of me.. I'm stationary.
}


void EnemyTurret::OnCollisionTile( Tile* collidingTile ) {
    UNUSED( collidingTile );
    // Not sure yet
}


void EnemyTurret::UpdateChaseTarget( float deltaSeconds, float targetDegrees, bool hasLoS ) {
    float maxDD = ENEMYTURRET_TOP_TURN_SPEED * deltaSeconds;
    m_orientationTopDegrees = GetTurnedTowards( m_orientationTopDegrees, targetDegrees, maxDD );

    if( fabsf( GetAngulaDisplacement( m_orientationTopDegrees, targetDegrees ) ) <= 5.f && hasLoS ) {
        ShootGun();
    }
}


void EnemyTurret::UpdateTurretVerts() {
    m_turretBaseVerts.clear();
    m_turretTopVerts.clear();

    // Base Verts
    AddVertsForAABB2D( m_turretBaseVerts, m_turretVertOffsets, Rgba::WHITE, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_turretBaseVerts, 1.f, m_orientationDegrees, m_position );

    // Top Verts
    AddVertsForAABB2D( m_turretTopVerts, m_turretVertOffsets, Rgba::WHITE, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_turretTopVerts, 1.f, m_orientationTopDegrees, m_position );

    if( g_theGame->IsDebugDrawingOn() ) {
        UpdateDebugVerts();
    }

}


void EnemyTurret::UpdateLaserVerts() {
    m_laserVerts.clear();

    Vec2 direction = Vec2::MakeFromPolarDegrees( m_orientationTopDegrees );
    RaycastResult raycast = m_map->Raycast( m_position, direction, ENEMYTURRET_MAX_SIGHT_RANGE );
    Vec2 laserEnd;

    if( raycast.DidImpact() ) {
        laserEnd = raycast.impactPosition;
    } else {
        laserEnd = m_position + (direction * ENEMYTURRET_MAX_SIGHT_RANGE);
    }

    AddVertsForLine2D( m_laserVerts, m_position, laserEnd, 0.05f, Rgba( 1.f, 0.f, 0.f, 0.75f ) );
}


const Vec2 EnemyTurret::GetForwardVectorTop() const {
    return Vec2( CosDegrees( m_orientationTopDegrees ), SinDegrees( m_orientationTopDegrees ) );
}


void EnemyTurret::ShootGun() {
    if( m_gunCooldown <= 0 ) {
        SoundID shootSound = g_theAudio->CreateOrGetSound( AUDIO_ENEMY_SHOOT );
        g_theAudio->PlaySound( shootSound );

        Vec2 barrelPosition = TransformPosition( GetForwardVectorTop(), 0.5f, 0.f, m_position );
        m_map->SpawnNewEntity( ENTITY_TYPE_BULLET, barrelPosition, m_orientationTopDegrees, (Entity*)this );
        m_map->SpawnNewExplosion( barrelPosition, EXPLOSION_SCALE_SMALL, 0.25f );

        m_gunCooldown = ENEMYTURRET_RATE_OF_FIRE_SECONDS;
    }
}
