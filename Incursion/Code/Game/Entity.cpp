#include "Game/Entity.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"


Entity::Entity( EntityType type /*= ENTITY_TYPE_UNKNOWN*/, FactionID faction /*= FACTION_UNKNOWN */ ) :
    m_entityType(type) {
    SetFaction( faction );
}


const Vec2 Entity::GetPosition() const {
    return m_position;
}


const FactionID Entity::GetFaction() const {
    return m_faction;
}


const EntityType Entity::GetEntityType() const {
    return m_entityType;
}


bool Entity::IsAlive() const {
    return !m_isDead;
}


bool Entity::IsGarbage() const {
    return m_isGarbage;
}


bool Entity::IsKillable() const {
    return m_isKillable;
}


bool Entity::IsSolid() const {
    return m_isSolid;
}


bool Entity::IsMovable() const {
    return m_isMovable;
}


void Entity::GetPhysicsDisc( Vec2& position, float& radius ) const {
    position = m_position;
    radius = m_physicsRadius;
}


void Entity::GetCosmeticDist( Vec2& position, float& radius ) const {
    position = m_position;
    radius = m_cosmeticRadius;
}


void Entity::SetFaction( FactionID faction ) {
    m_faction = faction;

    switch(m_faction) {
        // Intentional fallthrough for all player faction entities
        // All using the same audio clip
        case(FACTION_PLAYER0): {
        } case(FACTION_PLAYER1): {
        } case(FACTION_PLAYER2): {
        } case(FACTION_PLAYER3): {
            m_hitSound = g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_HIT );
            break;
        } case( FACTION_ENEMY1 ): {
            m_hitSound = g_theAudio->CreateOrGetSound( AUDIO_ENEMY_HIT );
            break;
        }
    }
}


void Entity::TakeDamage( int damageToTake ) {
    g_theAudio->PlaySound( m_hitSound );
    m_health -= damageToTake;

    if( m_health <= 0 ) {
        Die();
    }
}


Vec2 Entity::GetForwardVector() const {
    return Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) );
}


void Entity::UpdateDebugVerts() {
    m_debugCosmeticVerts.clear();
    m_debugPhysicsVerts.clear();

    AddVertsForRing2D( m_debugCosmeticVerts, m_position, m_cosmeticRadius, 0.05f, m_debugCosmeticColor );
    AddVertsForRing2D( m_debugPhysicsVerts, m_position, m_physicsRadius, 0.05f, m_debugPhysicsColor );
}
