#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Math/AABB2.hpp"

#include "Game/Entity.hpp"


class Map;
class Texture;

class Bullet
    : public Entity {
    public:
    explicit Bullet( Map* map, Entity* source);
    void Die();

    void Startup();
    void Shutdown();

    void Update( float deltaSeconds );
    void Render() const;

    void OnCollisionEntity( Entity* collidingEntity );
    void OnCollisionTile( Tile* collidingTile );

    private:
    const Entity* m_source = nullptr;
    Texture* m_bulletTexture = nullptr;
    std::vector<Vertex_PCU> m_bulletVerts;
    AABB2 m_bulletVertOffsets = AABB2();

    int m_destructionCountdown = BULLET_LIFETIME_BOUNCES;

    void UpdateBulletVerts();
};