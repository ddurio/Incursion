#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"


class Camera {
	public:
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
    Vec2 GetDimensions() const;

    Vec2 GetCenter() const;
    void GetCenter( float& centerX_out, float& centerY_out ) const;

    void Translate2D( const Vec2& translation2D );

	private:
	Vec2 m_BottomLeft;
	Vec2 m_TopRight;

};
