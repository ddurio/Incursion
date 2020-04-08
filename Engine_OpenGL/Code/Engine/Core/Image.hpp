#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVec2.hpp"


class Image {
    public:
    explicit Image( const char* imageFilePath );
    explicit Image( std::string imageFilePath );
    ~Image();

    IntVec2 GetDimensions() const;
    Rgba GetTexelColor( int texelX, int texelY ) const;
    Rgba GetTexelColor( const IntVec2& texelCoords ) const;

    std::string GetImageFilePath() const;
    unsigned char* GetRawData() const;
    int GetNumComponents() const;
    void GetRotated( int numRotations, Image& out_rotatedImage ) const;

    void SetTexelColor( int texelX, int texelY, const Rgba& texelColor );
    void SetTexelColor( const IntVec2& texelCoords, const Rgba& texelColor );

    private:
    std::string m_imageFilePath = "";
    IntVec2 m_dimensions = IntVec2::ZERO;
    std::vector<Rgba> m_texels;
    unsigned char* m_rawData = nullptr;
    bool m_isRawDataValid = false;
    int m_numComponents = 0;
};