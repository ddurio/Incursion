#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"


class Vertex_PCU {
	public:
	Vec3 m_position;
	Rgba m_color;
	Vec2 m_uvTexCoords;

	Vertex_PCU();
	explicit Vertex_PCU( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords );

	private:

};
