#include "Game/RaycastResult.hpp"


bool RaycastResult::DidImpact() const {
    return (impactTile != nullptr);
}
