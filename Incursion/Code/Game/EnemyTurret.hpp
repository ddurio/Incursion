#pragma once
#include "vector"
#include "Engine/Math/AABB2.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"


class Map;
class Texture;

class EnemyTurret
    : public Entity {
    public:
    explicit EnemyTurret( Map* map );
    void Die();

    void Startup();
    void Shutdown();

    void Update( float deltaSeconds );
    void Render() const;

    void OnCollisionEntity( Entity* collidingEntity );
    void OnCollisionTile( Tile* collidingTile );

    private:
    Entity* m_target = nullptr;
    float m_scanForTarget = -1.f;
    Vec2 m_targetLastKnownPosition = Vec2::ZERO;
    bool m_scanLeft = true;

    Texture* m_baseTexture = nullptr;
    Texture* m_topTexture = nullptr;
    AABB2 m_turretVertOffsets = AABB2();
    std::vector<Vertex_PCU> m_turretBaseVerts = {};
    std::vector<Vertex_PCU> m_turretTopVerts = {};
    std::vector<Vertex_PCU> m_laserVerts = {};
    float m_orientationTopDegrees = 0.f;
    float m_gunCooldown = 0.f;

    void UpdateChaseTarget( float deltaSeconds, float targetDegrees, bool hasLoS );
    void UpdateTurretVerts();
    void UpdateLaserVerts();
    const Vec2 GetForwardVectorTop() const;
    void ShootGun();
};