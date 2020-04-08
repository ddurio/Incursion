#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Math/AABB2.hpp"

#include "Game/Entity.hpp"


class Map;
class Texture;

class Boulder : public Entity {
    public:
    explicit Boulder( Map* map );
    void Die();

    void Startup();
    void Shutdown();

    void Update( float deltaSeconds );
    void Render() const;

    void OnCollisionEntity( Entity* collidingEntity );
    void OnCollisionTile( Tile* collidingTile );

    private:
    const Texture* m_texture = nullptr;
    Vec2 m_uvMins = Vec2::ZERO;
    Vec2 m_uvMaxs = Vec2::ONE;
    AABB2 m_boulderVertOffsets = AABB2();
    std::vector<Vertex_PCU> m_boulderVerts;

    void UpdateBoulderVerts();
};