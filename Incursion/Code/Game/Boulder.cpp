#include "Game/Boulder.hpp"

#include "Engine/core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"


Boulder::Boulder( Map* map ) :
    Entity( ENTITY_TYPE_BOULDER,FACTION_UNKNOWN ) {
    m_map = map;
}


void Boulder::Die() {
    m_isDead = true;
    m_isGarbage = true;
}


void Boulder::Startup() {
    m_isKillable = false;

    m_velocity = Vec2( 0.f, 0.f );
    m_orientationDegrees = 0.f;

    m_physicsRadius = BOULDER_PHYSICS_RADIUS;
    m_cosmeticRadius = BOULDER_COSMETIC_RADIUS;

    m_texture = g_theGame->GetExtrasTexture();
    g_theGame->GetExtrasSprite( BOULDER_SPRITE_INDEX ).GetUVs( m_uvMins, m_uvMaxs );
    m_boulderVertOffsets = AABB2( Vec2( -BOULDER_COSMETIC_BOX_OFFSET, -BOULDER_COSMETIC_BOX_OFFSET ), Vec2( BOULDER_COSMETIC_BOX_OFFSET, BOULDER_COSMETIC_BOX_OFFSET ) );

    UpdateBoulderVerts();
}


void Boulder::Shutdown() {

}


void Boulder::Update( float deltaSeconds ) {
    UNUSED( deltaSeconds );
}


void Boulder::Render() const {
    if( g_theGame->IsDebugDrawingOn() ) {
        g_theRenderer->BindTexture( nullptr );
        g_theRenderer->DrawVertexArray( m_debugCosmeticVerts );
    }

    g_theRenderer->BindTexture( m_texture );
    g_theRenderer->DrawVertexArray( m_boulderVerts );

    if( g_theGame->IsDebugDrawingOn() ) {
        g_theRenderer->BindTexture( nullptr );
        g_theRenderer->DrawVertexArray( m_debugPhysicsVerts );
    }
}


void Boulder::OnCollisionEntity( Entity* collidingEntity ) {
    UNUSED( collidingEntity );
}


void Boulder::OnCollisionTile( Tile* collidingTile ) {
    if( collidingTile->IsSolid() ) {
        PushDiscOutOfAABB2( m_position, m_physicsRadius, collidingTile->GetBounds() );
        UpdateBoulderVerts();
    }
}


void Boulder::UpdateBoulderVerts() {
    m_boulderVerts.clear();

    // Base Verts
    AddVertsForAABB2D( m_boulderVerts, m_boulderVertOffsets, Rgba::WHITE, m_uvMins, m_uvMaxs );
    TransformVertexArray( m_boulderVerts, 1.f, m_orientationDegrees, m_position );

    if( g_theGame->IsDebugDrawingOn() ) {
        UpdateDebugVerts();
    }
}

