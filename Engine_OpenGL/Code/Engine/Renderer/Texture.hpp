#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/IntVec2.hpp"

class Texture {
    friend class RenderContext;

    public:

    unsigned int GetTextureID() const;
    IntVec2 GetDimensions() const;
    const Image* GetImage() const;

    private:
    explicit Texture( const char* imageFilePath );
    ~Texture();

    unsigned int m_textureID = 0;
    const char* m_imageFilePath;
    IntVec2 m_dimensions = IntVec2::ZERO;
    Image* m_image = nullptr;
};