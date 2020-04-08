#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include "map"


struct Rgba {
	public:
	float r = 1.f;
	float g = 1.f;
	float b = 1.f;
	float a = 1.f;

	Rgba();
	explicit Rgba( float r, float g, float b, float a = 1.f );
    explicit Rgba( const char* initialValues );

	bool operator==( const Rgba& compare ) const;				// vec2 == vec2
	bool operator!=( const Rgba& compare ) const;				// vec2 != vec2

    const std::string GetAsString() const;

    void SetFromText( const char* valuesAsText );
    void SetFromBytes( float redByte, float greenByte, float blueByte, float alphaByte = 255.f );

    static const Rgba BLACK;
    static const Rgba BLUE;
    static const Rgba CYAN;
    static const Rgba GRAY;
    static const Rgba GREEN;
    static const Rgba MAGENTA;
    static const Rgba PURPLE;
    static const Rgba RED;
    static const Rgba WHITE;
    static const Rgba YELLOW;

    static const std::map<std::string, const Rgba&> s_colors;
};
