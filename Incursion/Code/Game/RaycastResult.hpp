#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"


class Tile;

struct RaycastResult {
    public:
    float impactFraction = 0.f;
    Vec2 impactPosition = Vec2::ONE;
    Vec2 impactNormal = Vec2::ONE;
    float impactDistance = 0.f;
    const Tile* impactTile;

    bool DidImpact() const;
};