#include "Engine/Renderer/Texture.hpp"


unsigned int Texture::GetTextureID() const {
    return m_textureID;
}


IntVec2 Texture::GetDimensions() const {
    return m_dimensions;
}


const Image* Texture::GetImage() const {
    return m_image;
}


Texture::Texture( const char* imageFilePath ) :
    m_imageFilePath(imageFilePath) {
    m_image = new Image( m_imageFilePath );
    m_dimensions = m_image->GetDimensions();
}


Texture::~Texture() {
    delete m_image;
    m_image = nullptr;
}
