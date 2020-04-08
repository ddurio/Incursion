#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "map"
#include "vector"


enum DrawMode {
    DRAW_MODE_ALPHA,
    DRAW_MODE_ADDITIVE
};

class Texture;
class BitmapFont;

class RenderContext {
	public:
	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

    Texture* CreateOrGetTextureFromFile( const char* imageFilePath );
    void BindTexture( const Texture* texture ) const;

    BitmapFont* CreateOrGetBitmapFontFromFile( const char* fontName );

	void ClearScreen( const Rgba& clearColor );
	void BeginCamera( const Camera& camera );
	void EndCamera( const Camera& camera );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes, DrawMode mode = DRAW_MODE_ALPHA );
    void DrawVertexArray( std::vector<Vertex_PCU> vertexes, DrawMode mode = DRAW_MODE_ALPHA );

	Vec2 currentCameraBottomLeft;
	Vec2 currentCameraTopRight;

	private:
    std::map<std::string, Texture*> m_loadedTextures;
    std::map<std::string, BitmapFont*> m_loadedFonts;
	//HGLRC m_apiRenderingContext = nullptr;  //SD1Fixme: Needed after moving code to the WindowContext

    DevConsoleChannel m_consoleChannel = 0x00;

    Texture* CreateTextureFromFile( const char* imageFilePath );
    BitmapFont* CreateBitmapFontFromFile( const char* fontName );
};
