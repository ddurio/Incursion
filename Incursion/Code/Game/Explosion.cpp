#include "Game/Explosion.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDef.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Game/Game.hpp"


Explosion::Explosion( Map* map, const Vec2& position, float scale, float duration /*= 1.f */ ) :
    Entity( ENTITY_TYPE_EXPLOSION, FACTION_UNKNOWN ),
    m_scale(scale),
    m_duration(duration) {
    m_map = map;
    m_position = position;
}


void Explosion::Die() {
    m_isDead = true;
    m_isGarbage = true;
}


void Explosion::Startup() {
    m_cosmeticRadius = 0.75f * m_scale;
    m_physicsRadius = 0.f;
    m_explosionVertOffsets = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );

    m_texture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_EXPLOSION );
    SpriteSheet* explosionSheet = new SpriteSheet( m_texture, IntVec2(5, 5) );
    m_anim = new SpriteAnimDef( *explosionSheet, 0, 24, m_duration, SPRITE_ANIM_PLAYBACK_ONCE );
}


void Explosion::Shutdown() {

}


void Explosion::Update( float deltaSeconds ) {
    m_age += deltaSeconds;

    const SpriteDef& sprites =  m_anim->GetSpriteDefAtTime( m_age );

    Vec2 uvMins;
    Vec2 uvMaxs;
    sprites.GetUVs( uvMins, uvMaxs );

    m_explosionVerts.clear();
    AddVertsForAABB2D( m_explosionVerts, m_explosionVertOffsets, Rgba::WHITE, uvMins, uvMaxs );
    TransformVertexArray( m_explosionVerts, m_scale, m_orientationDegrees, m_position );

    if( g_theGame->IsDebugDrawingOn() ) {
        UpdateDebugVerts();
    }

    if( m_age >= m_duration ) {
        Die();
    }
}


void Explosion::Render() const {
    if( !m_isDead ) {
        if( g_theGame->IsDebugDrawingOn() ) {
            g_theRenderer->BindTexture( nullptr );
            g_theRenderer->DrawVertexArray( m_debugCosmeticVerts );
        }

        g_theRenderer->BindTexture( m_texture );
        g_theRenderer->DrawVertexArray( m_explosionVerts, DRAW_MODE_ADDITIVE );

        if( g_theGame->IsDebugDrawingOn() ) {
            g_theRenderer->BindTexture( nullptr );
            g_theRenderer->DrawVertexArray( (int)m_debugPhysicsVerts.size(), m_debugPhysicsVerts.data() );
        }
    }
}


void Explosion::OnCollisionEntity( Entity* collidingEntity ) {
    UNUSED( collidingEntity );
    return;
}


void Explosion::OnCollisionTile( Tile* collidingTile ) {
    UNUSED( collidingTile );
    return;
}
