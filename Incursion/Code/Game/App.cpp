#include "Game/App.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"


App* g_theApp;
Game* g_theGame;

RenderContext* g_theRenderer;
InputSystem* g_theInput;
AudioSystem* g_theAudio;
RNG* g_RNG;

App::App() {

}


App::~App() {

}


void App::Startup() {
    g_RNG = new RNG();

	g_theRenderer = new RenderContext();
	g_theRenderer->Startup();

    g_theInput = new InputSystem();
    g_theInput->Startup();

    g_theAudio = new AudioSystem();
    g_theAudio->Startup();

	g_theGame = new Game( m_gamePreload );
	g_theGame->Startup();
}


void App::Shutdown() {
	g_theGame->Shutdown();
	delete g_theGame;
	g_theGame = nullptr;

    g_theAudio->Shutdown();
    delete g_theAudio;
    g_theAudio = nullptr;

    g_theInput->Shutdown();
    delete g_theInput;
    g_theInput = nullptr;

	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;

    delete g_RNG;
    g_RNG = nullptr;
}


void App::RunFrame() {
	BeginFrame();
	Update();
	Render();
	EndFrame();
}


void App::BeginFrame() {
    g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
    g_theAudio->BeginFrame();
}


void App::EndFrame() {
    g_theInput->EndFrame();
	g_theRenderer->EndFrame();
    g_theAudio->EndFrame();
}


void App::Update() {
    double currentTime = GetCurrentTimeSeconds();
    float deltaSeconds = (float)(currentTime - m_timeLastFrame);
    m_timeLastFrame = currentTime;
    deltaSeconds = ClampFloat( deltaSeconds, 0.f, APP_MAX_DELTA_SECONDS );

	if( m_isSlowMo ) {
		deltaSeconds /= 10.f;
	} else if( m_isFastMo ) {
        deltaSeconds *= 4.f;
    }

	g_theGame->Update( deltaSeconds );
}


void App::Render() const {
	g_theRenderer->ClearScreen( Rgba( 0.f, 0.f, 0.f, 1.f ) );

	g_theGame->Render();
}


bool App::HandleKeyPressed( unsigned char keyCode ) {
	switch( keyCode ) {
        case(0x77): { // F8 Key
            Shutdown();
            m_gamePreload = false;
            Startup();
            return 0;
        } case('T'): {
            m_isSlowMo = true;
            return 0;
        } case('Y'): {
            m_isFastMo = true;
            return 0;
        }
	}

	return g_theGame->HandleKeyPressed( keyCode );
}


bool App::HandleKeyReleased( unsigned char keyCode ) {
	switch( keyCode ) {
        case('T'): {
            m_isSlowMo = false;
            return 0;
        } case('Y'): {
            m_isFastMo = false;
            return 0;
        }
	}

	return g_theGame->HandleKeyReleased( keyCode );
}


bool App::HandleQuitRequested() {
	m_isQuitting = true;
	return 0;
}
