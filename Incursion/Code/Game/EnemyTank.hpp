#pragma once
#include "vector"
#include "Engine/Math/AABB2.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"


class Map;
class Texture;

class EnemyTank
    : public Entity {
    public:
    explicit EnemyTank( Map* map );
    void Die();

    void Startup();
    void Shutdown();

    void Update( float deltaSeconds );
    void Render() const;

    void OnCollisionEntity( Entity* collidingEntity );
    void OnCollisionTile( Tile* collidingTile );

    private:
    Entity* m_target = nullptr;
    Vec2 m_targetLastKnownPosition = Vec2::ZERO;
    bool m_investigateTarget = false;

    Texture* m_baseTexture = nullptr;
    Texture* m_topTexture = nullptr;
    AABB2 m_tankVertOffsets = AABB2();
    std::vector<Vertex_PCU> m_tankBaseVerts = {};
    std::vector<Vertex_PCU> m_tankTopVerts = {};

    float m_desiredOrientationDegrees = 0.f;
    float m_orientationTopDegrees = 0.f;
    float m_gunCooldown = 0.f;

    void UpdateChaseTarget( float deltaSeconds, float targetDegrees, bool hasLoS );
    void UpdateWanderAround( float deltaSeconds );
    void UpdateTankVerts();
    const Vec2 GetForwardVectorTop() const;
    void ShootGun();
};
