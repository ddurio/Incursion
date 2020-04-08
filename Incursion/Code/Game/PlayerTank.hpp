#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/AABB2.hpp"
#include "vector"

class Map;
class Texture;

class PlayerTank
    : public Entity {

    public:
    explicit PlayerTank( Map* map, int playerID );
    void Die();

    void Startup();
    void StartupExtraLife( int lifeIndex );
    void Shutdown();

    void Update( float deltaSeconds );
    void UpdateExtraLife( float deltaSeconds );
    void Render() const;

    //bool HandleKeyPressed( unsigned char keyCode );
    //bool HandleKeyReleased( unsigned char keyCode );

    int GetPlayerID() const;
    bool HasLivesRemaing() const;

    void OnCollisionEntity( Entity* collidingEntity );
    void OnCollisionTile( Tile* collidingTile );

    void SetStartPosition();
    void SetInvincible( bool isInvincible );

    private:
    const int m_playerID = -1;

    Texture* m_baseTexture = nullptr;
    Texture* m_topTexture = nullptr;

    bool m_isThrusting = false;
    float m_thrustFraction = 0.f;

    float m_desiredOrientationBaseDegrees = 0.f;
    float m_desiredOrientationTopDegrees = 0.f;
    float m_orientationTopDegrees = 0.f;

    AABB2 m_tankVertOffsets = AABB2();
    std::vector<Vertex_PCU> m_tankBaseVerts;
    std::vector<Vertex_PCU> m_tankTopVerts;

    float m_scale = 1.f;
    float m_deathCountdown = 3.f;
    float m_gunCooldown = PLAYERTANK_RATE_OF_FIRE_SECONDS;

    PlayerTank* m_extraLives[PLAYERTANK_EXTRA_LIVES] = {};
    int m_extraLifeIndex = -1;

    void StartupTexture();
    void UpdateFromController( float deltaSeconds );
    void UpdateTankVerts();

    const Vec2 GetForwardVectorTop() const;

    void ShootGun();
    void Respawn();

    void UseExtraLife();
};