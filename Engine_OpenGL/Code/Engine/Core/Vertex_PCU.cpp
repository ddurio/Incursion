#include "Engine/Core/Vertex_PCU.hpp"


Vertex_PCU::Vertex_PCU() {

}


Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords ) {
	this->m_position = position;
	this->m_color = color;
	this->m_uvTexCoords = uvTexCoords;
}
