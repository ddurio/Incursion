#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "vector"

typedef std::vector<Vertex_PCU> VertexList;

void AddVertsForDisc2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, const Rgba& color, int numSides = 64 );
void AddVertsForLine2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, const Rgba& color );
void AddVertsForRing2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides = 64 );
void AddVertsForAABB2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba& color, const Vec2& uvAtMins = Vec2( 0.f, 0.f ), const Vec2& uvAtMaxs = Vec2( 1.f, 1.f ) );

void TransformVertex( Vertex_PCU& position, float scaleXY, float rotationDegreesAboutZ, const Vec2& translationXY );
void TransformVertexArray( int numVertices, Vertex_PCU* position, float scaleXY, float rotationDegreesAboutZ, const Vec2& translationXY );
void TransformVertexArray( std::vector<Vertex_PCU>& vertexArray, float scaleXY, float rotationDegreesAboutZ, const Vec2& translationXY );
