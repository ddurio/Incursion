#include "Game/Bullet.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"
#include "Game/Tile.hpp"


Bullet::Bullet( Map* map, Entity* source ) :
    Entity( ENTITY_TYPE_BULLET, source->GetFaction() ),
    m_source(source) {
    m_map = map;
}


void Bullet::Die() {
    m_map->SpawnNewExplosion( m_position, EXPLOSION_SCALE_SMALL );

    m_isDead = true;
    m_isGarbage = true;
}


void Bullet::Startup() {
    m_velocity.x = BULLET_MAX_SPEED * CosDegrees( m_orientationDegrees );
    m_velocity.y = BULLET_MAX_SPEED * SinDegrees( m_orientationDegrees );

    m_isKillable = false;
    m_isSolid = false;

    m_physicsRadius = BULLET_PHYSICS_RADIUS;
    m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
    m_bulletTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_BULLET );

    m_bulletVertOffsets = AABB2( Vec2( -BULLET_COSMETIC_BOX_OFFSET, -BULLET_COSMETIC_BOX_OFFSET ), Vec2( BULLET_COSMETIC_BOX_OFFSET, BULLET_COSMETIC_BOX_OFFSET ) );

    UpdateBulletVerts();
}


void Bullet::Shutdown() {

}


void Bullet::Update( float deltaSeconds ) {
    if( m_map->GetTileFromWorldCoords( m_position ).IsSolid() ) {
        Die();
    }

    float distanceX = m_velocity.x * deltaSeconds;
    float distanceY = m_velocity.y * deltaSeconds;

    Vec2 translation = Vec2( distanceX, distanceY );

    Vec2 newPosition = m_position + translation;
    Tile newTile = m_map->GetTileFromWorldCoords( newPosition );

    if( newTile.IsSolid() ) {
        OnCollisionTile( &newTile );

        distanceX = m_velocity.x * deltaSeconds;
        distanceY = m_velocity.y * deltaSeconds;

        translation = Vec2( distanceX, distanceY );
    }

    m_position += translation;
    UpdateBulletVerts();
}


void Bullet::Render() const {
    if( g_theGame->IsDebugDrawingOn() ) {
        g_theRenderer->BindTexture( nullptr );
        g_theRenderer->DrawVertexArray( m_debugCosmeticVerts );
    }

    g_theRenderer->BindTexture( m_bulletTexture );
    g_theRenderer->DrawVertexArray( m_bulletVerts );

    if( g_theGame->IsDebugDrawingOn() ) {
        g_theRenderer->BindTexture( nullptr );
        g_theRenderer->DrawVertexArray( m_debugPhysicsVerts );
    }
}


void Bullet::OnCollisionEntity( Entity* collidingEntity ) {
    EntityType collidingType = collidingEntity->GetEntityType();
    if( collidingType == ENTITY_TYPE_BULLET || collidingType == ENTITY_TYPE_EXPLOSION ) {
        return;
    }

    // Check if can take damage, hurt/reflect
    if( collidingEntity->GetFaction() != m_faction ) {
        if( collidingEntity->IsKillable() ) {
            collidingEntity->TakeDamage( 1 );
            Die();
        } else {
            Vec2 collidingPosition;
            float collidingRadius;
            collidingEntity->GetPhysicsDisc( collidingPosition, collidingRadius );
            PushDiscOutOfDisc( m_position, m_physicsRadius, collidingPosition, collidingRadius );

            Vec2 normal = collidingPosition - m_position;
            normal.Normalize();

            m_velocity = GetReflectedVector( m_velocity, normal );
            m_orientationDegrees = m_velocity.GetAngleDegrees();

            m_destructionCountdown--;
            if( m_destructionCountdown <= 0 ) {
                Die();
            }
        }
    }
}


void Bullet::OnCollisionTile( Tile* collidingTile ) {
    // Unlike most entities, this only called when a reflection is needed
    // Determined by Bullet's Update instead of Map
    AABB2 tileBounds = collidingTile->GetBounds();
    Vec2 contactPoint = tileBounds.GetClosestPointOnAABB2( m_position );
    Vec2 normal = m_position - contactPoint;
    normal.Normalize();

    m_velocity = GetReflectedVector( m_velocity, normal );
    m_orientationDegrees = m_velocity.GetAngleDegrees();

    m_destructionCountdown--;
    if( m_destructionCountdown <= 0 ) {
        Die();
    }
}


void Bullet::UpdateBulletVerts() {
    m_bulletVerts.clear();

    // Base Verts
    AddVertsForAABB2D( m_bulletVerts, m_bulletVertOffsets, Rgba::WHITE, Vec2::ZERO, Vec2::ONE );
    TransformVertexArray( m_bulletVerts, 1.f, m_orientationDegrees, m_position );

    if( g_theGame->IsDebugDrawingOn() ) {
        UpdateDebugVerts();
    }
}
