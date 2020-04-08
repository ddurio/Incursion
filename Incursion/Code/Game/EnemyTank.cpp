#include "Game/EnemyTank.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/RaycastResult.hpp"


EnemyTank::EnemyTank( Map* map ) :
    Entity( ENTITY_TYPE_ENEMYTANK, FACTION_ENEMY1 ) {
    m_map = map;
}



void EnemyTank::Die() {
    SoundID deathSound = g_theAudio->CreateOrGetSound( AUDIO_ENEMY_DIE );
    g_theAudio->PlaySound( deathSound );

    m_map->SpawnNewExplosion( m_position, EXPLOSION_SCALE_LARGE );

    m_isDead = true;
    m_isGarbage = true;
}


void EnemyTank::Startup() {
    m_velocity = Vec2( 0, 0 );
    m_orientationTopDegrees = m_orientationDegrees; // orientation set by map before calling Startup in SpawnNewEntity
    m_health = ENEMYTANK_MAX_HEALTH;

    m_isMovable = false;

    m_physicsRadius = ENEMYTANK_PHYSICS_RADIUS;
    m_cosmeticRadius = ENEMYTANK_COSMETIC_RADIUS;

    m_baseTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTANK_BASE );
    m_topTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTANK_TOP );

    m_tankVertOffsets = AABB2( Vec2( -ENEMYTANK_COSMETIC_BOX_OFFSET, -ENEMYTANK_COSMETIC_BOX_OFFSET ), Vec2( ENEMYTANK_COSMETIC_BOX_OFFSET, ENEMYTANK_COSMETIC_BOX_OFFSET ) );

    UpdateTankVerts();
}


void EnemyTank::Shutdown() {
    m_isDead = true;
    m_isGarbage = true;
}


void EnemyTank::Update( float deltaSeconds ) {
    m_gunCooldown -= deltaSeconds;

    if( m_target == nullptr || !m_target->IsAlive() ) {
        m_target = m_map->AcquireNewTarget();

        if( m_target == nullptr ) {
            UpdateWanderAround( deltaSeconds );
            UpdateTankVerts();
            return;
        }
    }

    bool hasLoS = m_map->HasLineOfSight( this, m_target );
    Vec2 targetDisplacement = (m_target->GetPosition() - m_position);

    if( hasLoS && targetDisplacement.GetLength() < ENEMYTANK_MAX_SIGHT_RANGE ) { // Have LoS, Chase
        m_investigateTarget = true;
        m_targetLastKnownPosition = m_target->GetPosition();

        UpdateChaseTarget( deltaSeconds, targetDisplacement.GetAngleDegrees(), hasLoS );
    } else if( m_investigateTarget ) { // No LoS, Investigate last known position
        targetDisplacement = m_targetLastKnownPosition - m_position;
        UpdateChaseTarget( deltaSeconds, targetDisplacement.GetAngleDegrees(), hasLoS );

        if( targetDisplacement.GetLengthSquared() < 0.01f ) {
            m_investigateTarget = false;
        }
    } else {
        UpdateWanderAround( deltaSeconds );
    }

    UpdateTankVerts();
}


void EnemyTank::Render() const {
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
        g_theRenderer->DrawVertexArray( m_debugPhysicsVerts );
    }
}


void EnemyTank::OnCollisionEntity( Entity* collidingEntity ) {
    if( collidingEntity->IsSolid() ) {
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


void EnemyTank::OnCollisionTile( Tile* collidingTile ) {
    if( collidingTile->IsSolid() ) {
        PushDiscOutOfAABB2( m_position, m_physicsRadius, collidingTile->GetBounds() );
        UpdateTankVerts();
    }
}


void EnemyTank::UpdateChaseTarget( float deltaSeconds, float targetDegrees, bool hasLoS ) {
    float maxDD = ENEMYTANK_TURN_SPEED * deltaSeconds;
    m_orientationDegrees = GetTurnedTowards( m_orientationDegrees, targetDegrees, maxDD );

    maxDD = ENEMYTANK_TOP_TURN_SPEED * deltaSeconds;
    m_orientationTopDegrees = GetTurnedTowards( m_orientationTopDegrees, targetDegrees, maxDD );

    if( fabsf( GetAngulaDisplacement( m_orientationDegrees, targetDegrees ) ) <= 45.f ) {
        float speed = ENEMYTANK_MAX_SPEED * deltaSeconds;
        m_position += speed * GetForwardVector();
    }

    if( fabsf( GetAngulaDisplacement( m_orientationTopDegrees, targetDegrees ) ) <= 5.f && hasLoS ) {
        ShootGun();
    }
}


void EnemyTank::UpdateWanderAround( float deltaSeconds ) {
    Vec2 lNormVec = Vec2::MakeFromPolarDegrees( m_orientationDegrees + ENEMYTANK_WHISKER_ANGLE );
    Vec2 rNormVec = Vec2::MakeFromPolarDegrees( m_orientationDegrees - ENEMYTANK_WHISKER_ANGLE );
    Vec2 cNormVec = Vec2::MakeFromPolarDegrees( m_orientationDegrees );

    RaycastResult lWhisker = m_map->Raycast( m_position, lNormVec, ENEMYTANK_WHISKER_RANGE );
    RaycastResult rWhisker = m_map->Raycast( m_position, rNormVec, ENEMYTANK_WHISKER_RANGE );
    RaycastResult cWhisker = m_map->Raycast( m_position, cNormVec, m_cosmeticRadius );

    bool lImpact = lWhisker.DidImpact();
    bool rImpact = rWhisker.DidImpact();
    bool cImpact = cWhisker.DidImpact();

    if( cImpact ) { // Basically Running into something
        m_desiredOrientationDegrees = fmodf( m_orientationDegrees + 179.f, 360.f ); // Almost about face, always turn left
        float maxDD = ENEMYTANK_TURN_SPEED * deltaSeconds;
        m_orientationDegrees = GetTurnedTowards( m_orientationDegrees, m_desiredOrientationDegrees, maxDD );
    } else if( !lImpact && !rImpact ) { // Zero whisker impact
        float speed = ENEMYTANK_MAX_SPEED * deltaSeconds;
        m_position += speed * GetForwardVector();
    } else if( lImpact ^ rImpact ) { // One whisker impact
        float speed = ENEMYTANK_MAX_SPEED * deltaSeconds;

        float turnSpeed = ENEMYTANK_TURN_SPEED * deltaSeconds;
        float turnDegrees = rImpact ? turnSpeed : -turnSpeed;
        m_orientationDegrees += turnDegrees;

        float topTurnSpeed = ENEMYTANK_TOP_TURN_SPEED * deltaSeconds;
        float turnTopDegrees = rImpact ? topTurnSpeed : -topTurnSpeed;
        m_orientationTopDegrees += turnTopDegrees;

        m_position += speed * GetForwardVector();
    } else if( lImpact && rImpact ) { // Two whiskers impact
        float turnSpeed = ENEMYTANK_TURN_SPEED * deltaSeconds;

        bool turnLeft = lWhisker.impactDistance > rWhisker.impactDistance;
        float turnDegrees = turnLeft ? turnSpeed : -turnSpeed;
        m_orientationDegrees += turnDegrees;

        float topTurnSpeed = ENEMYTANK_TOP_TURN_SPEED * deltaSeconds;
        float turnTopDegrees = turnLeft ? topTurnSpeed : -topTurnSpeed;
        m_orientationTopDegrees += turnTopDegrees;

        float speed = ENEMYTANK_MAX_SPEED * deltaSeconds;
        m_position += speed * GetForwardVector();
    }
}


void EnemyTank::UpdateTankVerts() {
    m_tankBaseVerts.clear();
    m_tankTopVerts.clear();

    // Base Verts
    AddVertsForAABB2D( m_tankBaseVerts, m_tankVertOffsets, Rgba::WHITE, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_tankBaseVerts, 1.f, m_orientationDegrees, m_position );

    // Top Verts
    AddVertsForAABB2D( m_tankTopVerts, m_tankVertOffsets, Rgba::WHITE, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_tankTopVerts, 1.f, m_orientationTopDegrees, m_position );

    if( g_theGame->IsDebugDrawingOn() ) {
        UpdateDebugVerts();

        Vec2 lWhisker = Vec2::MakeFromPolarDegrees( m_orientationDegrees + ENEMYTANK_WHISKER_ANGLE, ENEMYTANK_WHISKER_RANGE );
        Vec2 rWhisker = Vec2::MakeFromPolarDegrees( m_orientationDegrees - ENEMYTANK_WHISKER_ANGLE, ENEMYTANK_WHISKER_RANGE );
        Vec2 cWhisker = Vec2::MakeFromPolarDegrees( m_orientationDegrees, m_cosmeticRadius );

        AddVertsForLine2D( m_debugCosmeticVerts, m_position, m_position + lWhisker, 0.05f, Rgba( 1.f, 0.f, 0.f, 0.75f ) );
        AddVertsForLine2D( m_debugCosmeticVerts, m_position, m_position + rWhisker, 0.05f, Rgba( 1.f, 0.f, 0.f, 0.75f ) );
        AddVertsForLine2D( m_debugCosmeticVerts, m_position, m_position + cWhisker, 0.05f, Rgba( 1.f, 0.f, 0.f, 0.75f ) );
    }

}


const Vec2 EnemyTank::GetForwardVectorTop() const {
    return Vec2( CosDegrees( m_orientationTopDegrees ), SinDegrees( m_orientationTopDegrees ) );
}


void EnemyTank::ShootGun() {
    if( m_gunCooldown <= 0 ) {
        SoundID shootSound = g_theAudio->CreateOrGetSound( AUDIO_ENEMY_SHOOT );
        g_theAudio->PlaySound( shootSound );

        Vec2 barrelPosition = TransformPosition( GetForwardVectorTop(), 0.4f, 0.f, m_position );
        m_map->SpawnNewEntity( ENTITY_TYPE_BULLET, barrelPosition, m_orientationTopDegrees, (Entity*)this );
        m_map->SpawnNewExplosion( barrelPosition, EXPLOSION_SCALE_SMALL, 0.25f );

        m_gunCooldown = ENEMYTANK_RATE_OF_FIRE_SECONDS;
    }
}
