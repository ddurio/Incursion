#include "Engine/Core/Image.hpp"

#pragma warning( disable: 4100 ) // Unreferenced formal parameter
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"


Image::Image( const char* imageFilePath ) :
    m_imageFilePath(imageFilePath) {
    int imageTexelSizeX = 0; // Filled in for us to indicate image width
    int imageTexelSizeY = 0; // Filled in for us to indicate image height
    int numComponents = 0; // Filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
    int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

    stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
    m_rawData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

    // Check if the load was successful
    GUARANTEE_OR_DIE( m_rawData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
    GUARANTEE_OR_DIE( (numComponents == 3 || numComponents == 4) && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );

    m_isRawDataValid = true;
    m_numComponents = numComponents;
    m_dimensions = IntVec2( imageTexelSizeX, imageTexelSizeY );

    int numTexels = m_dimensions.x * m_dimensions.y;
    m_texels.resize( numTexels );

    for( int texelIndex = 0; texelIndex < numTexels; texelIndex++ ) {
        // Get Byte Indices
        int redIndex = texelIndex * numComponents;
        int greenIndex = redIndex + 1;
        int blueIndex  = redIndex + 2;
        int alphaIndex = redIndex + 3; // Not always used

        // Get Byte Data
        float redByte = m_rawData[redIndex];
        float greenByte = m_rawData[greenIndex];
        float blueByte = m_rawData[blueIndex];
        float alphaByte = 255.f;

        if( numComponents == 4 ) {
            alphaByte = m_rawData[alphaIndex];
        }

        // Set Rgba Values
        m_texels[texelIndex].SetFromBytes( redByte, greenByte, blueByte, alphaByte );
    }
}


Image::Image( std::string imageFilePath ) :
    Image(imageFilePath.c_str()) {
}


Image::~Image() {
    stbi_image_free( m_rawData );
}


IntVec2 Image::GetDimensions() const {
    return m_dimensions;
}


Rgba Image::GetTexelColor( int texelX, int texelY ) const {
    int texelIndex = texelY * m_dimensions.x + texelX;
    return m_texels[texelIndex];
}


Rgba Image::GetTexelColor( const IntVec2& texelCoords ) const {
    return GetTexelColor( texelCoords.x, texelCoords.y );
}


std::string Image::GetImageFilePath() const {
    return m_imageFilePath;
}


unsigned char* Image::GetRawData() const {
    if( !m_isRawDataValid ) {
        // recreate rawData?
    }

    return m_rawData;
}


int Image::GetNumComponents() const {
    return m_numComponents;
}


void Image::GetRotated( int numRotations, Image& out_rotatedImage ) const {
    out_rotatedImage = *this;

    for( int rotationIndex = 0; rotationIndex < numRotations; rotationIndex++ ) {
        int numTexels = m_dimensions.x * m_dimensions.y;
        std::vector<Rgba> texelCopies = out_rotatedImage.m_texels;

        for( int texelIndex = 0; texelIndex < numTexels; texelIndex++ ) {
            int texelX = texelIndex % m_dimensions.x;
            int texelY = texelIndex / m_dimensions.x;

            int newTexelX = m_dimensions.y - 1 - texelY;
            int newTexeY = texelX;

            int newTexelIndex = newTexeY * m_dimensions.x + newTexelX;

            out_rotatedImage.m_texels[newTexelIndex] = texelCopies[texelIndex];
        }
    }
}


void Image::SetTexelColor( int texelX, int texelY, const Rgba& texelColor ) {
    int texelIndex = texelY * m_dimensions.x + texelX;
    m_texels[texelIndex] = texelColor;
}


void Image::SetTexelColor( const IntVec2& texelCoords, const Rgba& texelColor ) {
    SetTexelColor( texelCoords.x, texelCoords.y, texelColor );
}
