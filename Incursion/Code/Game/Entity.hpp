#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec2.hpp"
#include "vector"


enum EntityType {
    ENTITY_TYPE_UNKNOWN = -1,

    ENTITY_TYPE_BOULDER,
    ENTITY_TYPE_ENEMYTANK,
    ENTITY_TYPE_ENEMYTURRET,
    ENTITY_TYPE_PLAYERTANK,
    ENTITY_TYPE_BULLET,
    ENTITY_TYPE_EXPLOSION,

    NUM_ENTITY_TYPES
};

enum FactionID {
    FACTION_UNKNOWN = -1,
    FACTION_PLAYER0,
    FACTION_PLAYER1,
    FACTION_PLAYER2,
    FACTION_PLAYER3,

    FACTION_ENEMY1,

    NUM_FACTIONS
};

class Tile;


class Entity {
    friend class Map;

	public:
    explicit Entity( EntityType type = ENTITY_TYPE_UNKNOWN, FactionID faction = FACTION_UNKNOWN );
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;

	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() const = 0;

    virtual void Die() = 0;
    void TakeDamage( int damageToTake );

    bool IsAlive() const;
    bool IsGarbage() const;
    bool IsKillable() const;
    bool IsSolid() const;
    bool IsMovable() const;

    const Vec2 GetPosition() const;
    const FactionID GetFaction() const;
    const EntityType GetEntityType() const;
    void GetPhysicsDisc( Vec2& position, float& radius) const;
    void GetCosmeticDist( Vec2& position, float& radius) const;

    void SetFaction( FactionID faction );
    virtual void OnCollisionEntity( Entity* collidingEntity ) = 0;
    virtual void OnCollisionTile( Tile* collidingTile ) = 0;

	protected:
    const EntityType m_entityType = ENTITY_TYPE_UNKNOWN;
    FactionID m_faction = FACTION_UNKNOWN;
    Rgba m_factionTint = Rgba::WHITE;

    Map* m_map = nullptr;
    SoundID m_hitSound = MISSING_SOUND_ID;

	Vec2 m_position;
	Vec2 m_velocity;

    float m_angularVelocity = 0;
    float m_orientationDegrees = 0;

    float m_physicsRadius = 0.f;
	float m_cosmeticRadius = 0.f;

	int m_health = 1;
    bool m_isKillable = true;
    bool m_isSolid = true;
    bool m_isMovable = true;

    bool m_isDead = false;
    bool m_isGarbage = false;

    const Rgba m_debugCosmeticColor = Rgba( 1, 0, 1, 1 );
    const Rgba m_debugPhysicsColor = Rgba( 0, 1, 1, 1 );

    std::vector<Vertex_PCU> m_debugCosmeticVerts;
    std::vector<Vertex_PCU> m_debugPhysicsVerts;

    Vec2 GetForwardVector() const;
    void UpdateDebugVerts();
};
