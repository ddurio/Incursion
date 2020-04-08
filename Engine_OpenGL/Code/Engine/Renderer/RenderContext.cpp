#include "Engine/Renderer/RenderContext.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"


void RenderContext::Startup() {
    m_consoleChannel  = g_theDevConsole->AddChannelByName( "RenderContext" );
    m_consoleChannel |= DevConsole::CHANNEL_INFO;

    g_theDevConsole->PrintString( "(Renderer) Startup Begun...", Rgba::MAGENTA, m_consoleChannel );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    g_theDevConsole->PrintString( "(Renderer) Startup Complete", Rgba::MAGENTA, m_consoleChannel );
}


void RenderContext::Shutdown() {
    g_theDevConsole->PrintString( "(Renderer) Shutdown Begun...", Rgba::MAGENTA, m_consoleChannel );
    std::map<std::string, Texture*>::iterator textureIter = m_loadedTextures.begin();

    for( textureIter; textureIter != m_loadedTextures.end(); textureIter++ ) {
        delete textureIter->second;
        textureIter->second = nullptr;
    }

    std::map<std::string, BitmapFont*>::iterator fontIter = m_loadedFonts.begin();

    for( fontIter; fontIter != m_loadedFonts.end(); fontIter++ ) {
        delete fontIter->second;
        fontIter->second = nullptr;
    }

    m_loadedTextures.clear();
    m_loadedFonts.clear();

    g_theDevConsole->PrintString( "(Renderer) Shutdown Complete", Rgba::MAGENTA, m_consoleChannel );
}


void RenderContext::BeginFrame() {

}


void RenderContext::EndFrame() {

}


Texture* RenderContext::CreateOrGetTextureFromFile( const char* imageFilePath ) {
    std::map<std::string, Texture*>::const_iterator textureIter = m_loadedTextures.find( imageFilePath );

    if( textureIter == m_loadedTextures.end() ) {
        return CreateTextureFromFile( imageFilePath );
    } else {
        return textureIter->second;
    }
}



void RenderContext::BindTexture( const Texture* texture ) const {
    if( texture ) {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, texture->GetTextureID() );
    } else {
        glDisable( GL_TEXTURE_2D );
    }
}


BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile( const char* fontName ) {
    std::map<std::string, BitmapFont*>::iterator fontIter = m_loadedFonts.find( fontName );

    if( fontIter == m_loadedFonts.end() ) {
        return CreateBitmapFontFromFile( fontName );
    } else {
        return fontIter->second;
    }
}


void RenderContext::ClearScreen( const Rgba& clearColor ) {
	// Clear all screen (back buffer) pixels to black
	glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
	glClear( GL_COLOR_BUFFER_BIT );
}


void RenderContext::BeginCamera( const Camera& camera ) {
	currentCameraBottomLeft = camera.GetOrthoBottomLeft();
	currentCameraTopRight = camera.GetOrthoTopRight();

	glLoadIdentity();
	glOrtho( currentCameraBottomLeft.x, currentCameraTopRight.x, currentCameraBottomLeft.y, currentCameraTopRight.y, 0.f, 1.f );
}


void RenderContext::EndCamera( const Camera& camera ) {
	UNUSED( camera );
}


void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes, DrawMode mode /*= DRAW_MODE_MULTIPLICATIVE*/ ) {
	const Vertex_PCU* vert;

    switch( mode ) {
        case(DRAW_MODE_ALPHA): {
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            break;
        } case(DRAW_MODE_ADDITIVE): {
            glBlendFunc( GL_SRC_ALPHA, GL_ONE );
            break;
        }
    }

	glBegin( GL_TRIANGLES );
	for( int i = 0; i < numVertexes; i++ ) {
		vert = &vertexes[i];
		glColor4f( vert->m_color.r, vert->m_color.g, vert->m_color.b, vert->m_color.a );
        glTexCoord2f( vert->m_uvTexCoords.x, vert->m_uvTexCoords.y );
		glVertex2f( vert->m_position.x, vert->m_position.y );
	}
	glEnd();
}


void RenderContext::DrawVertexArray( std::vector<Vertex_PCU> vertexes, DrawMode mode /*= DRAW_MODE_MULTIPLICATIVE*/ ) {
    int size = (int)vertexes.size();
    const Vertex_PCU* data = vertexes.data();
    DrawVertexArray( size, data, mode );
}


Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath ) {
    g_theDevConsole->PrintString( Stringf( "(Renderer) Loading new texture from file (%s)...", imageFilePath ), Rgba::MAGENTA, m_consoleChannel );

    Texture* newTexture = new Texture( imageFilePath );
    m_loadedTextures.insert( { imageFilePath, newTexture } );

    int imageTexelSizeX = newTexture->m_dimensions.x;
    int imageTexelSizeY = newTexture->m_dimensions.y;
    unsigned char* imageData = newTexture->m_image->GetRawData();

    // Enable OpenGL texturing
    glEnable( GL_TEXTURE_2D );

    // Tell OpenGL that our pixel data is single-byte aligned
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    // Ask OpenGL for an unused texName (ID number) to use for this texture
    glGenTextures( 1, (GLuint*)&newTexture->m_textureID );

    // Tell OpenGL to bind (set) this as the currently active texture
    glBindTexture( GL_TEXTURE_2D, newTexture->m_textureID );

    // Set texture clamp vs. wrap (repeat) default settings
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ); // GL_CLAMP or GL_REPEAT
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ); // GL_CLAMP or GL_REPEAT

    // Set magnification (texel > pixel) and minification (texel < pixel) filters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

    // Pick the appropriate OpenGL format (RGB or RGBA) for this texel data
    int numComponents = newTexture->GetImage()->GetNumComponents();
    GLenum bufferFormat = (numComponents == 3) ? GL_RGB : GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
    GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

    // Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
    glTexImage2D(			// Upload this pixel data to our new OpenGL texture
        GL_TEXTURE_2D,		// Creating this as a 2d texture
        0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
        internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
        imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
        imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
        0,					// Border size, in texels (must be 0 or 1, recommend 0)
        bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
        GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
        imageData );		// Address of the actual pixel data bytes/buffer in system memory

    return newTexture;
}


BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* fontName ) {
    g_theDevConsole->PrintString( Stringf( "(Renderer) Loading new font from file (%s)...", fontName ), Rgba::MAGENTA, m_consoleChannel );

    std::string imageFilePath = Stringf( "Data/Fonts/%s.png", fontName );
    Texture* texture = CreateOrGetTextureFromFile( imageFilePath.c_str() );

    BitmapFont* font = new BitmapFont( fontName, texture );
    m_loadedFonts.insert( { fontName, font } );

    return font;
}
