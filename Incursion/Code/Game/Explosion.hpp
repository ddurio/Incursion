#pragma once
#include "Engine/Math/AABB2.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"


class SpriteAnimDef;
class Texture;

class Explosion : public Entity {
    public:
    explicit Explosion( Map* map, const Vec2& position, float scale, float duration = EXPLOSION_DURATION );

    void Die();

    void Startup();
    void Shutdown();

    void Update( float deltaSeconds );
    void Render() const;

    void OnCollisionEntity( Entity* collidingEntity );
    void OnCollisionTile( Tile* collidingTile );

    private:
    SpriteAnimDef* m_anim;
    float m_scale = 1.f;
    float m_duration = 0.f;
    float m_age = 0.0f;

    const Texture* m_texture = nullptr;
    AABB2 m_explosionVertOffsets = AABB2();
    std::vector<Vertex_PCU> m_explosionVerts;
};
