#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"


enum GameMode {
    GAME_MODE_UNKNOWN = -1,

    GAME_MODE_VERSUS,
    GAME_MODE_COOP,

    NUM_GAME_MODES
};

enum LoadingState {
    LOADING_INIT,
    LOADING_ASSETS,
    LOADING_COMPLETE
};

class BitmapFont;
class Texture;
class SpriteDef;
class SpriteSheet;

class Game {
	public:
	explicit Game( bool doPreLoading = true );
	~Game();

	void Startup();
	void Shutdown();

	void Update( float deltaSeconds );
	void Render() const;
    bool IsDebugDrawingOn() const;
    void SetCameraShakeAmount( float newCameraShakeAmount );

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	bool HandleQuitRequested();

    void ReturnToAttractScreen();
    void EndGame();
    void ToggleGameAudio();

    const Vec2 GetRandomPointJustOffscreen( float entityRadius );

    bool HasGameBeenBeaten() const;
    bool IsPlayerTankCollisionEnabled() const;
    const Camera& GetActiveCamera() const;
    const Texture* GetExtrasTexture() const;
    const SpriteDef& GetExtrasSprite( int spriteIndex ) const;
    GameMode GetGameMode() const;

    void StartNextMap();

	private:
    LoadingState m_loadingState = LOADING_INIT;

    bool m_onAttractScreen = false;
    SoundPlaybackID m_attractAudioID = MISSING_SOUND_ID;
    float m_attractAudioDelay = GAME_ATTRACT_AUDIO_DELAY;
    bool m_attractAudioIsPlaying = false;

    bool m_onEndScreen = false;
    bool m_hasBeatenTheGame = false;
    float m_endScreenTimer = GAME_END_SCREEN_TIME_SECONDS;
    GameMode m_gameMode = GAME_MODE_VERSUS;
    SoundPlaybackID m_gameAudioID = MISSING_SOUND_ID;

    bool m_debugDrawing = false;
    float m_cameraShakeAmount = 0.f;

    bool m_useDebugCamera = false;
    Camera* m_playerCamera = nullptr;
    Camera* m_debugCamera = nullptr;

    bool m_debugPlayerTankCollision = false;
    bool m_isPaused = false;

    std::vector<Map*> m_maps = {};
    Map* m_activeMap = nullptr;
    PlayerTank* m_extraLives[PLAYERTANK_EXTRA_LIVES * MAX_CONTROLLERS] = {};
    std::vector<Vertex_PCU> m_attractVerts;
    std::vector<Vertex_PCU> m_pauseVerts;
    std::vector<Vertex_PCU> m_pauseTextVerts;

    Texture* m_extrasTexture = nullptr;
    SpriteSheet* m_extrasSprites = nullptr;

    void StartupLoading();
    void StartupAttract();
    void StartupGame();
    void LoadAssets();
    void StartupTextures();
    void StartupSounds();

    void UpdateLoadingScreen( float deltaSeconds );
    void UpdateAttractScreen( float deltaSeconds );
    void UpdateGame( float deltaSeconds );

    void UpdateFromController( float deltaSeconds );

    void UpdateCameraPosition( float deltaSeconds );
    void UpdateDebugCamera( float deltaSeconds );
    void UpdatePlayerCamera( float deltaSeconds );
    void GetPlayerCameraCenteredOnPlayers( Vec2& outCameraCenter, std::vector<Vec2>& outPlayerPositions );
    void UpdateCameraShake( float deltaSeconds );

    void RenderLoadingScreen() const;
    void RenderAttractScreen() const;
    void RenderGame() const;
    void RenderEndScreen() const;
};
