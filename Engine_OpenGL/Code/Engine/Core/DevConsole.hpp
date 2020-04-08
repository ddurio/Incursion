#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DevConsoleLine.hpp"


class Camera;
class RenderContext;

enum DevConsoleMode {
    DEV_CONSOLE_OFF,
    DEV_CONSOLE_FULLSCREEN,
    //DEV_CONSOLE_COMPACT
};

class DevConsole {
    public:
    DevConsole() {};
    ~DevConsole() {};

    void Startup();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    bool HandleKeyPressed( unsigned char keyCode );

    void Render( RenderContext* renderer, const Camera& camera, float lineHeight );

    bool IsTakingInput() const;
    DevConsoleChannel GetChannelFromString( const std::string& inputString ) const;

    DevConsoleChannel AddChannelByName( const std::string& inputName, bool enableChannel = true );
    void SwitchToChannel( DevConsoleChannel channel );
    void SwitchToChannel( const std::string& channelName );
    void SetActiveChannels( int numChannels, ... );
    void SetActiveChannels( const std::string& channelNamesCSV );

    void PrintString( std::string inputString, const Rgba& inputColor = CONSOLE_INFO, DevConsoleChannel inputChannel = CHANNEL_UNDEFINED );
    void PrintString( std::string inputString, const Rgba& inputColor, const std::string& inputChannelName );
    void ExecuteCommandString( const std::string& commandString );
    void Toggle();

    static const Rgba& CONSOLE_COMMAND;
    static const Rgba& CONSOLE_INFO;
    static const Rgba& CONSOLE_WARNING;
    static const Rgba& CONSOLE_ERROR;

    static constexpr DevConsoleChannel CHANNEL_UNDEFINED = 0x00000000;
    static constexpr DevConsoleChannel CHANNEL_COMMAND   = 0x00000001;
    static constexpr DevConsoleChannel CHANNEL_INFO      = 0x00000002;
    static constexpr DevConsoleChannel CHANNEL_WARNING   = 0x00000004;
    static constexpr DevConsoleChannel CHANNEL_ERROR     = 0x00000008;

    static bool Command_Help( EventArgs& args );
    static bool Command_Exec( EventArgs& args );
    static bool Command_Test( EventArgs& args );
    static bool Command_Echo( EventArgs& args );
    static bool Command_SetActiveChannels( EventArgs& args );

    void RunCommandTests();

    private:
    DevConsoleMode m_mode = DEV_CONSOLE_OFF;
    std::vector<DevConsoleLine> m_lines;
    //DevConsoleChannel m_activeChannels = 0xFFFFFFFF;  // Default all active
    DevConsoleChannel m_activeChannels = 0x00;

    static constexpr int MAX_CHANNELS = sizeof( DevConsoleChannel ) * 8;
    std::string m_channels[ MAX_CHANNELS ];

    int m_frameNumber = 0;

    void ExecuteCommandLine( const std::string& command );
    void SetActiveChannels( const Strings& channels );
    void EnableChannel( int channelIndex );
    void DisableChannel( int channelIndex );
};
