#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/VertexUtils.hpp"

#include "vector"


enum TextDrawMode {
    TEXT_DRAW_OVERRUN,
    TEXT_DRAW_SHRINK_TO_FIT
};

struct AABB2;
class SpriteSheet;
class Texture;
struct Vec2;
class Vertex_PCU;

class BitmapFont {
    friend class RenderContext;

    private:
    // Only RenderContext can make a new font
    explicit BitmapFont( const char* fontName, const Texture* fontTexture );

    public:
    const Texture* GetTexture() const;
    void AddVertsForText2D( VertexList& textVerts, const Vec2& textPositionMins, float cellHeight, const std::string& text, const Rgba& tint = Rgba::WHITE, float cellAspect = 1.f, int maxGlyphs = 9999999 ) const;
    void AddVeretsForTextInBox2D( VertexList& textVerts, const AABB2& boxBounds, float cellHeight, const std::string& text, const Rgba& tint = Rgba::WHITE, float cellAspect = 1.f, const Vec2& alignment = ALIGN_CENTER, TextDrawMode mode = TEXT_DRAW_OVERRUN, int maxGlyphs = 9999999 ) const;

    private:
    const char* m_name = "";
    SpriteSheet* m_glyphSpriteSheet = nullptr;

    float GetGlyphAspect( int glyphUnicode ) const;
    Vec2 GetTextSize( float cellHeight, const std::string& text, float cellAspect ) const;
    Vec2 GetTextSize( float cellHeight, const std::vector<std::string>& text, float cellAspect ) const;
    float CalculateShrinkToFitScale( const Vec2& textDimensions, const Vec2& boundsDimensions ) const;
};
