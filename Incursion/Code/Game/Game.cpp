#include "Game/Game.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Game/App.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/TileDef.hpp"


Game::Game( bool doPreLoading /*= true */ ) {
    if( !doPreLoading ) {
        m_loadingState = LOADING_COMPLETE;
        m_onAttractScreen = false;
    }
}

Game::~Game() {

}


void Game::Startup() {
    m_playerCamera = new Camera();
    m_debugCamera = new Camera();

    if( m_loadingState != LOADING_COMPLETE ) {
        StartupLoading();
    } else if( m_onAttractScreen ) {
        StartupAttract();
    } else {
        StartupGame();
    }
}


void Game::Shutdown() {
    int numMaps = (int)m_maps.size();

    for( int mapIndex = 0; mapIndex < numMaps; mapIndex++ ) {
        Map* map = m_maps[mapIndex];
        if( map != nullptr ) {
            map->Shutdown();
            delete map;
            m_maps[mapIndex] = nullptr;
        }
    }
    m_maps.clear();

    TileDef::DestroyTileDefs();
}


void Game::Update( float deltaSeconds ) {
    if( m_isPaused ) {
        deltaSeconds = 0.f;
    }

    if( m_loadingState == LOADING_INIT ) {
        m_loadingState = LOADING_ASSETS;

        m_playerCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( 16.f, 9.f ) );
        UpdateLoadingScreen( deltaSeconds );
        return;
    } else if( m_loadingState == LOADING_ASSETS ) {
        m_loadingState = LOADING_COMPLETE;
        LoadAssets();

        m_onAttractScreen = true;

        SoundID loopID = g_theAudio->CreateOrGetSound( AUDIO_ATTRACT_LOOP );
        m_attractAudioID = g_theAudio->PlaySound( loopID, true );
    } else if( m_onAttractScreen ) {
        // FIXME: Attract screen camera ortho has not been set
        // FIXME: Attract screen camera can be set to debug camera... zoomed out
        UpdateAttractScreen( deltaSeconds );

        m_playerCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( 16.f, 9.f ) );
        
    } else if( m_onEndScreen ) {
        m_endScreenTimer -= deltaSeconds;

        if( m_endScreenTimer <= 0 ) {
            ReturnToAttractScreen();
        }
    } else {
        UpdateGame( deltaSeconds );

        if( m_onAttractScreen ) {
            // Back button pressed on controller while in game
            return;
        }

        UpdateCameraPosition( deltaSeconds );

        if( m_isPaused ) {
            const Camera& activeCamera = GetActiveCamera();
            Vec2 mins = activeCamera.GetOrthoBottomLeft();
            Vec2 maxs = activeCamera.GetOrthoTopRight();
            AABB2 cameraBounds = AABB2( mins, maxs );

            m_pauseVerts.clear();
            AddVertsForAABB2D( m_pauseVerts, cameraBounds, Rgba( 0.f, 0.f, 0.f, 0.5f ) );

            const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

            m_pauseTextVerts.clear();
            Vec2 cameraCenter = (mins + maxs) * 0.5f;

            std::string text = "Paused";
            float cellAspect = 0.6f;
            float cellHeight = 1.f;
            float cellWidth = cellAspect * cellHeight;
            float textStartX = cameraCenter.x - (cellWidth * text.size() * 0.5f);
            float textStartY = cameraCenter.y + (cameraBounds.GetDimensions().y * 0.25f);
            font->AddVertsForText2D( m_pauseTextVerts, Vec2( textStartX, textStartY ), cellHeight, text, Rgba::WHITE, cellAspect );

            text = "Press Start or P To Resume";
            cellHeight = 0.5f;
            cellWidth = cellAspect * cellHeight;
            textStartX = cameraCenter.x - (cellWidth * text.size() * 0.5f);
            textStartY = cameraCenter.y - (cameraBounds.GetDimensions().y * 0.25f);
            font->AddVertsForText2D( m_pauseTextVerts, Vec2( textStartX, textStartY ), cellHeight, text, Rgba::WHITE, cellAspect );
        }
    }
}


void Game::Render() const {

    Camera activeCamera = GetActiveCamera();
    g_theRenderer->BeginCamera( activeCamera );

    if( m_loadingState == LOADING_ASSETS ) {
        RenderLoadingScreen();
    } else if( m_onAttractScreen ) {
        RenderAttractScreen();
    } else if( m_onEndScreen ) {
        RenderEndScreen();
    } else {
        RenderGame();

        if( m_isPaused ) {
            g_theRenderer->BindTexture( nullptr );
            g_theRenderer->DrawVertexArray( m_pauseVerts );

            const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

            g_theRenderer->BindTexture( font->GetTexture() );
            g_theRenderer->DrawVertexArray( m_pauseTextVerts );
        }
    }

    g_theRenderer->EndCamera( activeCamera );
}


bool Game::IsDebugDrawingOn() const {
    return m_debugDrawing;
}


void Game::SetCameraShakeAmount( float newCameraShakeAmount ) {
    m_cameraShakeAmount = fmax( newCameraShakeAmount, CAMERA_SHAKE_MAX );
}


bool Game::HandleKeyPressed( unsigned char keyCode ) {
	switch( keyCode ) {
        case(0x1B): { // Escape Key
            if( m_onAttractScreen ) {
                return g_theApp->HandleQuitRequested();
            } else if( m_isPaused ) {
                ReturnToAttractScreen();
            } else {
                m_isPaused = true;
                ToggleGameAudio();
            }
            return 0;
        } case(0x70): { // F1 - Toggle Debug Drawing
            m_debugDrawing = !m_debugDrawing;
            return 0;
        } case(0x72): { // F3 - Toggle PlayerTank Collision
            m_debugPlayerTankCollision = !m_debugPlayerTankCollision;

            for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
                PlayerTank* player = m_activeMap->GetPlayer( playerIndex );
                if( player != nullptr ) {
                    player->SetInvincible( m_debugPlayerTankCollision );
                }
            }
            return 0;
        } case(0x73): { // F4 - Toggle Debug Camera
            if( !m_onAttractScreen && !m_onEndScreen ) {
                m_useDebugCamera = !m_useDebugCamera;
            } else {
                m_useDebugCamera = false;
            }
            return 0;
        } case(0x79): { // F10 - Go to Previous Level
            // not implemented yet
            return 0;
        } case(0x7A): { // F11 - Go to Next Level
            StartNextMap();
            return 0;
        } case('P'): { // Start, Pause, Unpause Game
            m_isPaused = !m_isPaused;
            ToggleGameAudio();
            return 0;
        } case('S'): { // Play Test Sound
            SoundID testSound = g_theAudio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
            g_theAudio->PlaySound( testSound );
        }
	}

    if( !m_onAttractScreen ) {
        return m_activeMap->HandleKeyPressed( keyCode );
    } else {
        return 1;
    }
}


bool Game::HandleKeyReleased( unsigned char keyCode ) {
    UNUSED( keyCode );

    if( !m_onAttractScreen ) {
	    //return m_thePlayerShip->HandleKeyReleased( keyCode );
        return 1;
    } else {
        return 1;
    }
}


bool Game::HandleQuitRequested() {
	return 1;
}


void Game::ReturnToAttractScreen() {
    Shutdown();
    StartupAttract();
}


void Game::EndGame() {
    if( HasGameBeenBeaten() || m_activeMap->AreAllPlayersDead() ) {
        m_onEndScreen = true;

        if( !HasGameBeenBeaten() ) {
            g_theAudio->StopSound( m_gameAudioID );
            SoundID gameOverID = g_theAudio->CreateOrGetSound( AUDIO_GAME_OVER );
            g_theAudio->PlaySound( gameOverID );

            SoundID attractID = g_theAudio->CreateOrGetSound( AUDIO_ATTRACT_LOOP );
            m_attractAudioID = g_theAudio->PlaySound( attractID );
        }
    }
}


void Game::ToggleGameAudio() {
    if( m_isPaused ) {
        if( m_gameAudioID != MISSING_SOUND_ID ) {
            g_theAudio->SetSoundPlaybackSpeed( m_gameAudioID, 0.f );
        }

        SoundID pauseID = g_theAudio->CreateOrGetSound( AUDIO_PAUSE );
        g_theAudio->PlaySound( pauseID );
    } else {
        SoundID unpauseID = g_theAudio->CreateOrGetSound( AUDIO_UNPAUSE );
        g_theAudio->PlaySound( unpauseID );
        g_theAudio->SetSoundPlaybackSpeed( m_gameAudioID, 1.f );
    }
}


const Vec2 Game::GetRandomPointJustOffscreen( float entityRadius ) {
    UNUSED( entityRadius );
    Vec2 tempXY = Vec2( 0.f, 0.f );
    float tempX = 0.f;
    float tempY = 0.f;
    /*
    int sideIndex = g_RNG->GetRandomIntLessThan( 4 );

    if( sideIndex == 0 ) { // Left
        tempX = -entityRadius;
        tempY = g_RNG->GetRandomFloatInRange( 0.f, WORLD_HEIGHT );
    } else if( sideIndex == 1 ) { // Right
        tempX = WORLD_WIDTH + entityRadius;
        tempY = g_RNG->GetRandomFloatInRange( 0.f, WORLD_HEIGHT );
    } else if( sideIndex == 2 ) { // Top
        tempX = g_RNG->GetRandomFloatInRange( 0.f, WORLD_WIDTH );
        tempY = WORLD_HEIGHT + entityRadius;
    } else { // Bottom
        tempX = g_RNG->GetRandomFloatInRange( 0.f, WORLD_WIDTH );
        tempY = -entityRadius;
    }
    */
    return Vec2( tempX, tempY );
}


bool Game::HasGameBeenBeaten() const {
    return m_hasBeatenTheGame;
}


bool Game::IsPlayerTankCollisionEnabled() const {
    return !m_debugPlayerTankCollision;
}


const Camera& Game::GetActiveCamera() const {
    Camera* activeCamera = m_useDebugCamera ? m_debugCamera : m_playerCamera;
    return *activeCamera;
}


const Texture* Game::GetExtrasTexture() const {
    return m_extrasTexture;
}


const SpriteDef& Game::GetExtrasSprite( int spriteIndex ) const {
    return m_extrasSprites->GetSpriteDef( spriteIndex );
}


GameMode Game::GetGameMode() const {
    return m_gameMode;
}


void Game::StartNextMap() {
    if( m_activeMap == m_maps.back() ) {
        if( !m_hasBeatenTheGame ) {
            g_theAudio->StopSound( m_gameAudioID );
            SoundID victoryID = g_theAudio->CreateOrGetSound( AUDIO_VICTORY );
            g_theAudio->PlaySound( victoryID );
        }

        m_hasBeatenTheGame = true;
        return;
    }

    int numMaps = (int)m_maps.size();

    for( int mapIndex = 0; mapIndex < numMaps; mapIndex++ ) {
        if( m_activeMap == m_maps[mapIndex] ) {
            Map* nextMap = m_maps[mapIndex + 1];
            m_activeMap->SendPlayersToNewMap( nextMap );

            m_activeMap = nextMap;
            m_activeMap->Startup();
            return;
        }
    }
}


void Game::StartupLoading() {
    g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

    SoundID anticipationID = g_theAudio->CreateOrGetSound( AUDIO_ANTICIPATION );
    g_theAudio->PlaySound( anticipationID );
}


void Game::StartupAttract() {
    m_onAttractScreen = true;

    // End Screen
    m_onEndScreen = false;
    m_endScreenTimer = GAME_END_SCREEN_TIME_SECONDS;

    // Gameplay
    m_isPaused = false;
    m_hasBeatenTheGame = false;
    m_useDebugCamera = false;

    // Audio
    m_attractAudioDelay = GAME_ATTRACT_AUDIO_DELAY;
    m_attractAudioIsPlaying = false;
}


void Game::StartupGame() {
    TileDef::InitializeTileDefs();
    StartupTextures();
    StartupSounds();

    m_extrasSprites = new SpriteSheet( m_extrasTexture, IntVec2( 4, 4 ) );

    m_hasBeatenTheGame = false;

    // Map Definitions
    std::map<TileType, float> tileFractions = {
        { TILE_TYPE_STONE, 0.1f }
    };
    std::map<EntityType, int> numEntities = {
        { ENTITY_TYPE_ENEMYTANK, 10 },
        { ENTITY_TYPE_ENEMYTURRET, 10 },
        { ENTITY_TYPE_BOULDER, 5 }
    };
    Map* map0 = new Map( IntVec2( 16, 30 ), TILE_TYPE_GRASS, TILE_TYPE_STONE, tileFractions, numEntities, false );

    tileFractions = {
        { TILE_TYPE_WOOD_BRICK, 0.3f },
        { TILE_TYPE_MUD, 0.6f }
    };
    numEntities = {
        { ENTITY_TYPE_ENEMYTANK, 20 },
        { ENTITY_TYPE_ENEMYTURRET, 20 },
        { ENTITY_TYPE_BOULDER, 15 }
    };
    Map* map1 = new Map( IntVec2( 30, 30 ), TILE_TYPE_SAND, TILE_TYPE_WOOD_BRICK, tileFractions, numEntities, false );

    tileFractions = {
        { TILE_TYPE_STONE_BRICK, 0.25f },
    };
    numEntities = {
        { ENTITY_TYPE_ENEMYTURRET, 30 },
        { ENTITY_TYPE_BOULDER, 10 }
    };
    Map* map2 = new Map( IntVec2( 47, 27 ), TILE_TYPE_ICE, TILE_TYPE_STONE_BRICK, tileFractions, numEntities, true );

    m_maps.push_back( map0 );
    m_maps.push_back( map1 );
    m_maps.push_back( map2 );
    m_activeMap = m_maps[0];

    if( m_attractAudioID != MISSING_SOUND_ID ) {
        g_theAudio->StopSound( m_attractAudioID );
    }

    SoundID gameMusic = g_theAudio->CreateOrGetSound( AUDIO_GAME_LOOP );
    m_gameAudioID = g_theAudio->PlaySound( gameMusic, true );

    m_activeMap->Startup();

}


void Game::LoadAssets() {
    StartupTextures();
    StartupSounds();
}


void Game::StartupTextures() {
    m_extrasTexture = g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_MAP_EXTRAS );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_MAP_TERRAIN );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE0 );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP0 );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE1 );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP1 );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE2 );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP2 );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_BASE3 );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_PLAYERTANK_TOP3 );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTANK_BASE );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTANK_TOP );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTURRET_BASE );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_ENEMYTURRET_TOP );

    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_BULLET );
    g_theRenderer->CreateOrGetTextureFromFile( TEXTURE_EXPLOSION );
}


void Game::StartupSounds() {
    // Anticipation loaded in StartupLoading
    g_theAudio->CreateOrGetSound( AUDIO_ATTRACT_LOOP );
    g_theAudio->CreateOrGetSound( AUDIO_GAME_LOOP );
    g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_JOIN );
    g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_DIE );
    g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_HIT );
    g_theAudio->CreateOrGetSound( AUDIO_PLAYERTANK_SHOOT );
    g_theAudio->CreateOrGetSound( AUDIO_ENEMY_DIE );
    g_theAudio->CreateOrGetSound( AUDIO_ENEMY_HIT );
    g_theAudio->CreateOrGetSound( AUDIO_ENEMY_SHOOT );
    g_theAudio->CreateOrGetSound( AUDIO_PAUSE );
    g_theAudio->CreateOrGetSound( AUDIO_UNPAUSE );
    g_theAudio->CreateOrGetSound( AUDIO_VICTORY );
    g_theAudio->CreateOrGetSound( AUDIO_GAME_OVER );
}


void Game::UpdateLoadingScreen( float deltaSeconds ) {
    UNUSED( deltaSeconds );

    const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

    m_attractVerts.clear();

    float cameraCenterX = (m_playerCamera->GetOrthoTopRight().x * 0.5f);
    float cameraCenterY = (m_playerCamera->GetOrthoTopRight().y * 0.5f);
    float cellAspect = 0.6f;

    std::string text = "Loading...";
    float cellHeight = 1.f;
    float cellWidth = cellAspect * cellHeight;
    float textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f);
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 1.5f ), cellHeight, text, Rgba::BLACK, cellAspect );
}


void Game::UpdateAttractScreen( float deltaSeconds ) {
    UpdateFromController( deltaSeconds );

    if( m_isPaused ) {
        StartupGame();
        m_onAttractScreen = false;
        m_isPaused = false;
        return;
    }


    const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

    m_attractVerts.clear();
    float cameraCenterX = (m_playerCamera->GetOrthoTopRight().x * 0.5f);
    float cameraCenterY = (m_playerCamera->GetOrthoTopRight().y * 0.5f);
    float cellAspect = 0.6f;

    std::string text = "Incursion";
    float cellHeight = 1.f;
    float cellWidth = cellAspect * cellHeight;
    float textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f);
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 1.5f ), cellHeight, text, Rgba::BLACK, cellAspect );

    text = "Danny Durio";
    cellHeight = 0.75f;
    cellWidth = cellAspect * cellHeight;
    textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f );
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 1.3f ), cellHeight, text, Rgba::BLUE, cellAspect );

    text = "Controls";
    cellHeight = 0.5f;
    cellWidth = cellAspect * cellHeight;
    textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f );
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 1.f ), cellHeight, text, Rgba::BLACK, cellAspect );

    text = "Move Tank: Left Joystick";
    cellHeight = 0.4f;
    cellWidth = cellAspect * cellHeight;
    textStartX = (cameraCenterX * 0.5f) - (cellWidth * text.size() * 0.5f );
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 0.8f ), cellHeight, text, Rgba::BLACK, cellAspect );

    text = "Rotate Gun: Right Joystick";
    textStartX = (cameraCenterX * 0.5f) - (cellWidth * text.size() * 0.5f );
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 0.6f ), cellHeight, text, Rgba::BLACK, cellAspect );

    text = "Fire Gun: A/Right Trigger";
    textStartX = (cameraCenterX * 1.5f) - (cellWidth * text.size() * 0.5f );
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 0.8f ), cellHeight, text, Rgba::BLACK, cellAspect );

    text = "Respawn: Y";
    textStartX = (cameraCenterX * 1.5f) - (cellWidth * text.size() * 0.5f );
    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 0.6f ), cellHeight, text, Rgba::BLACK, cellAspect );

    text = "Press P/Start to Begin";
    textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f );
    Rgba color = Rgba::GREEN;

    double time = GetCurrentTimeSeconds();
    float alpha = (sin( (float)time * 7.f + 1.f) + 1.f) * 0.5f;
    color.a = alpha;

    font->AddVertsForText2D( m_attractVerts, Vec2( textStartX, cameraCenterY * 0.2f ), cellHeight, text, color, cellAspect );
}


void Game::UpdateGame( float deltaSeconds ) {
    UpdateFromController( deltaSeconds );

    if( m_isPaused ) {
        deltaSeconds = 0.f;
    } else if( m_onAttractScreen ) {
        return;
    }

    Map* originalMap = m_activeMap;
    m_activeMap->Update( deltaSeconds );

    if( originalMap != m_activeMap ) { // Player moved to next map
        m_activeMap->Update( deltaSeconds );
    }

    // Shake Camera
    UpdateCameraShake( deltaSeconds );
}


void Game::UpdateFromController( float deltaSeconds ) {
    UNUSED( deltaSeconds );

    for( int i = 0; i < MAX_CONTROLLERS; i++ ) {
        const XboxController& controller = g_theInput->GetController( i );
        
        if( controller.IsConnected() ) {
            const KeyButtonState& startButton = controller.GetKeyButtonState( XBOX_BUTTON_ID_START );

            if( startButton.WasJustPressed() ) {
                m_isPaused = !m_isPaused;
                ToggleGameAudio();
            }

            const KeyButtonState& backButton = controller.GetKeyButtonState( XBOX_BUTTON_ID_SELECT );

            if( backButton.WasJustPressed() ) {
                if( m_onAttractScreen ) {
                    g_theApp->HandleQuitRequested();
                } else if( m_isPaused ) {
                    ReturnToAttractScreen();
                } else {
                    m_isPaused = true;
                }
            }
        }
    }
}


void Game::UpdateCameraPosition( float deltaSeconds ) {
    if( m_useDebugCamera ) {
        UpdateDebugCamera( deltaSeconds );
    } else {
        UpdatePlayerCamera( deltaSeconds );
    }

}


void Game::UpdateDebugCamera( float deltaSeconds ) {
    UNUSED( deltaSeconds );

    Camera* activeCamera = m_debugCamera;

    IntVec2 mapDimensions = m_activeMap->GetDimensions();
    float mapAspect = (float)mapDimensions.x / (float)mapDimensions.y;
    float cameraWidth;
    float cameraHeight;

    if( mapAspect > CLIENT_ASPECT ) { // Bound by width
    //if( mapDimensions.x > mapDimensions.y ) {
        cameraWidth = (float)mapDimensions.x;
        cameraHeight = cameraWidth / CLIENT_ASPECT;
    } else { // Bound by height
        cameraHeight = (float)mapDimensions.y;
        cameraWidth = CLIENT_ASPECT * cameraHeight;
    }

    Vec2 cameraMins = Vec2::ZERO;
    Vec2 cameraMaxs = Vec2( cameraWidth, cameraHeight );

    activeCamera->SetOrthoView( cameraMins, cameraMaxs );
}


void Game::UpdatePlayerCamera( float deltaSeconds ) {
    UNUSED( deltaSeconds );
    Camera* activeCamera = m_playerCamera;

    Vec2 cameraCenter = Vec2::ZERO;
    std::vector<Vec2> positions;

    GetPlayerCameraCenteredOnPlayers( cameraCenter, positions );

    float cameraWidth = CAMERA_PLAYER_WIDTH;
    float cameraHeight = CAMERA_PLAYER_HEIGHT;

    float halfCameraWidth = cameraWidth / 2;
    float halfCameraHeight = cameraHeight / 2;

    float mapWidth = (float)m_activeMap->GetDimensions().x;
    float mapHeight = (float)m_activeMap->GetDimensions().y;

    Vec2 farthestDistance = Vec2( halfCameraWidth, halfCameraHeight );
    int numPlayers = (int)positions.size();

    for( int playerIndex = 0; playerIndex < numPlayers; playerIndex++ ) {
        Vec2 distanceToPlayer = positions[playerIndex] - cameraCenter;

        if( distanceToPlayer.x > farthestDistance.x ) {
            farthestDistance.x = distanceToPlayer.x;
        }

        if( distanceToPlayer.y > farthestDistance.y ) {
            farthestDistance.y = distanceToPlayer.y;
        }
    }

    // Add buffer between player and edge of screen
    Vec2 bufferRadius = Vec2( PLAYERTANK_COSMETIC_RADIUS, PLAYERTANK_COSMETIC_RADIUS );

    if( farthestDistance.x + PLAYERTANK_COSMETIC_RADIUS > halfCameraWidth &&
        farthestDistance.y + PLAYERTANK_COSMETIC_RADIUS > halfCameraHeight ) {
        farthestDistance += bufferRadius;
    } else {
        farthestDistance = Vec2( halfCameraWidth, halfCameraHeight );
    }

    Vec2 cameraMins = cameraCenter - farthestDistance;
    Vec2 cameraMaxs = cameraCenter + farthestDistance;

    float desiredCameraWidth = cameraMaxs.x - cameraMins.x;
    float desiredCameraHeight = cameraMaxs.y - cameraMins.y;
    float currentAspect = desiredCameraWidth / desiredCameraHeight;

    // Preserve Aspect Ratio
    if( currentAspect > CLIENT_ASPECT ) {
        desiredCameraHeight = desiredCameraWidth / CLIENT_ASPECT;
        cameraMaxs.y = cameraMins.y + desiredCameraHeight;
    } else if( currentAspect < CLIENT_ASPECT ) {
        desiredCameraWidth = desiredCameraHeight * CLIENT_ASPECT;
        cameraMaxs.x = cameraMins.x + desiredCameraWidth;
    }

    // Lock camera onto the screen by shifting, clamping broke aspect ratio
    if( cameraMaxs.x - cameraMins.x < mapWidth ) {
        if( cameraMaxs.x > mapWidth ) {
            float difference = cameraMaxs.x - mapWidth;
            cameraMins.x -= difference;
            cameraMaxs.x -= difference;
        } else if( cameraMins.x < 0 ) {
            float difference = -cameraMins.x;
            cameraMins.x += difference;
            cameraMaxs.x += difference;
        }
    } else {
        if( cameraMins.x < 0 ) {
            float difference = -cameraMins.x;
            cameraMins.x += difference;
            cameraMaxs.x += difference;
        }
    }

    if( cameraMaxs.y - cameraMins.y < mapHeight ) {
        if( cameraMaxs.y > mapHeight ) {
            float difference = cameraMaxs.y - mapHeight;
            cameraMins.y -= difference;
            cameraMaxs.y -= difference;
        } else if( cameraMins.y < 0 ) {
            float difference = -cameraMins.y;
            cameraMins.y += difference;
            cameraMaxs.y += difference;
        }
    } else {
        if( cameraMins.y < 0 ) {
            float difference = -cameraMins.y;
            cameraMins.y += difference;
            cameraMaxs.y += difference;
        }
    }

	activeCamera->SetOrthoView( cameraMins, cameraMaxs );
}


void Game::GetPlayerCameraCenteredOnPlayers( Vec2& outCameraCenter, std::vector<Vec2>& outPlayerPositions ) {
    int numPlayers = 0;
    float cameraPositionX = 0;
    float cameraPositionY = 0;

    for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
        const XboxController& controller = g_theInput->GetController( playerIndex );
        
        if( controller.IsConnected() ) {
            const PlayerTank* player = m_activeMap->GetPlayer( playerIndex );

            if( player->IsAlive() ) {
                Vec2 playerPosition = player->GetPosition();
                cameraPositionX += playerPosition.x;
                cameraPositionY += playerPosition.y;
                outPlayerPositions.push_back( playerPosition );
                numPlayers++;
            }
        }
    }

    if( numPlayers != 0 ) {
        cameraPositionX /= numPlayers;
        cameraPositionY /= numPlayers;
    }

    outCameraCenter = Vec2( cameraPositionX, cameraPositionY );
}


void Game::UpdateCameraShake( float deltaSeconds ) {
    m_cameraShakeAmount -= CAMERA_SHAKE_REDUCTION_PER_SECOND * deltaSeconds;
    m_cameraShakeAmount = ClampFloat( m_cameraShakeAmount, 0.f, CAMERA_SHAKE_MAX );

    float randomX = g_RNG->GetRandomFloatInRange( 0.f, m_cameraShakeAmount );
    float randomY = g_RNG->GetRandomFloatInRange( 0.f, m_cameraShakeAmount );

    Camera activeCamera = GetActiveCamera();
    activeCamera.Translate2D( Vec2( randomX, randomY ) );
}


void Game::RenderLoadingScreen() const {
    g_theRenderer->ClearScreen( Rgba::CYAN );

    const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

    g_theRenderer->BindTexture( font->GetTexture() );
    g_theRenderer->DrawVertexArray( m_attractVerts );
}


void Game::RenderAttractScreen() const {
    g_theRenderer->ClearScreen( Rgba::CYAN );

    const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

    g_theRenderer->BindTexture( font->GetTexture() );
    g_theRenderer->DrawVertexArray( m_attractVerts );
}


void Game::RenderGame() const {
    m_activeMap->Render();
}


void Game::RenderEndScreen() const {
    Rgba color;
    VertexList fontVerts;

    const BitmapFont* font = g_theRenderer->CreateOrGetBitmapFontFromFile( FONT_NAME_SQUIRREL );

    float cameraCenterX;
    float cameraCenterY;
    m_playerCamera->GetCenter( cameraCenterX, cameraCenterY );

    float cellHeight = 1.f;
    float cellAspect = 0.6f;
    float cellWidth = cellAspect * cellHeight;

    if( m_hasBeatenTheGame ) {
        color = Rgba::GREEN;

        for( int playerIndex = 0; playerIndex < MAX_CONTROLLERS; playerIndex++ ) {
            PlayerTank* player = m_activeMap->GetPlayer( playerIndex );
            if( player != nullptr && player->IsAlive() ) {
                switch( playerIndex ) {
                    case(0): {
                        color = Rgba::BLUE;
                        break;
                    } case(1): {
                        color = Rgba::GREEN;
                        break;
                    } case(2): {
                        color = Rgba::YELLOW;
                        break;
                    } case(3): {
                        color = Rgba::PURPLE;
                        break;
                    }
                }
                break;
            }
        }

        std::string text = "CONGRATULATIONS!";

        float cameraHeight = m_playerCamera->GetDimensions().y;
        float textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f);
        float textStartY = cameraCenterY + (cameraHeight * 0.25f);

        font->AddVertsForText2D( fontVerts, Vec2( textStartX, textStartY ), cellHeight, text, Rgba::BLACK, cellAspect );
    } else {
        color = Rgba::RED;
        std::string text = "Game Over!";

        float cameraHeight = m_playerCamera->GetDimensions().y;
        float textStartX = cameraCenterX - (cellWidth * text.size() * 0.5f);
        float textStartY = cameraCenterY + (cameraHeight * 0.25f);

        font->AddVertsForText2D( fontVerts, Vec2( textStartX, textStartY ), cellHeight, text, Rgba::BLACK, cellAspect );
    }

    g_theRenderer->ClearScreen( color );
    g_theRenderer->BindTexture( font->GetTexture() );
    g_theRenderer->DrawVertexArray( fontVerts );

}
