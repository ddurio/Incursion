#include "Engine/Core/DevConsole.hpp"

#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "stdarg.h"


DevConsole* g_theDevConsole = new DevConsole();

const Rgba& DevConsole::CONSOLE_COMMAND = Rgba::CYAN;
const Rgba& DevConsole::CONSOLE_INFO    = Rgba::WHITE;
const Rgba& DevConsole::CONSOLE_WARNING = Rgba::YELLOW;
const Rgba& DevConsole::CONSOLE_ERROR   = Rgba::RED;


bool DevConsole::Command_Help( EventArgs& args ) {
    UNUSED( args );
    g_theDevConsole->PrintString( "Help Command Called" );

    Strings validCommands;
    g_theEventSystem->GetSubscribedEventsList( validCommands );

    int numCommands = (int)validCommands.size();

    for( int commandIndex = 0; commandIndex < numCommands; commandIndex++ ) {
        g_theDevConsole->PrintString( validCommands[commandIndex] );
    }

    return false;
}


bool DevConsole::Command_Exec( EventArgs& args ) {
    std::string fileName = args.GetValue( "file", "" );

    if( fileName == "" ) {
        g_theDevConsole->PrintString( "ERROR: Exec missing required argument \"file\"", CONSOLE_ERROR );
        g_theDevConsole->PrintString( "     - Usage Example: Exec file=<fileName>", CONSOLE_ERROR );
        return true;
    }

    XMLDocument document = new XMLDocument();
    const XMLElement& root = ParseXMLRootElement( fileName.c_str(), document );

    const char* tagName = "DevConsoleCommand";
    const XMLElement* element = root.FirstChildElement( tagName );

    /*  TODO: Switch to clearer text when Echo command supports spaces and quotes
    std::string errorText = Stringf( "ERROR: (%s) DevConsoleCommnad missing required argument \"command\"", fileName.c_str() );
    std::string usageText = "     - Usage Example: <DevConsoleCommand command=\"<validCommand>\"/>";
    std::string errorCommand = Stringf( "Echo text=\"%s\n%s\" color=RED", errorText.c_str(), usageText.c_str() );
    */
    std::string errorCommand = "Echo text=ERROR:InvalidFileCommand color=RED";

    for( element; element != nullptr; element = element->NextSiblingElement( tagName ) ) {
        std::string commandString = ParseXMLAttribute( *element, "command", errorCommand );
        g_theDevConsole->ExecuteCommandString( commandString );
    }

    return false;
}


bool DevConsole::Command_Test( EventArgs& args ) {
    g_theDevConsole->PrintString( "Test Command Called" );
    g_theDevConsole->PrintString( Stringf("- Args: \"%s\"", args.GetAsString().c_str() ) );
    return false;
}


//TODO: Improve to handle spaces and quotes
bool DevConsole::Command_Echo( EventArgs& args ) {
    std::string text          = args.GetValue( "text", "" );
    Rgba color                = args.GetValue( "color", CONSOLE_INFO );
    DevConsoleChannel channel = args.GetValue( "channel", CHANNEL_UNDEFINED );

    g_theDevConsole->PrintString( text, color, channel );

    return false;
}


void DevConsole::Startup() {
    AddChannelByName( "command" );
    AddChannelByName( "info" );
    AddChannelByName( "warning" );
    AddChannelByName( "error" );

    g_theEventSystem->SubscribeEventCallbackFunction( "Help", Command_Help );
    g_theEventSystem->SubscribeEventCallbackFunction( "Exec", Command_Exec );
    g_theEventSystem->SubscribeEventCallbackFunction( "Echo", Command_Echo );

    g_theEventSystem->SubscribeEventCallbackFunction( "Test", Command_Test );
}


void DevConsole::Shutdown() {

}


void DevConsole::BeginFrame() {
    m_frameNumber++;
}


void DevConsole::EndFrame() {

}


bool DevConsole::HandleKeyPressed( unsigned char keyCode ) {
    UNUSED( keyCode );
    return false;
}


void DevConsole::Render( RenderContext* renderer, const Camera& camera, float lineHeight ) {
    if( m_mode == DEV_CONSOLE_OFF ) {
        return;
    }

    std::string fontName = g_theGameConfigBlackboard.GetValue( "consoleFont", "" );

    if( fontName == "" ) {
        fontName = g_theGameConfigBlackboard.GetValue( "defaultFont", "" );
    }

    if( fontName == "" ) {
        ERROR_RECOVERABLE( "ERROR: No font found for DevConsole" );
        return;
    }

    const BitmapFont* font = renderer->CreateOrGetBitmapFontFromFile( fontName.c_str() );


    VertexList backgroundVerts;
    VertexList textVerts;

    AABB2 cameraBounds = AABB2( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() );
    AddVertsForAABB2D( backgroundVerts, cameraBounds, Rgba(0.f, 0.f, 0.f, 0.5f) );

    std::vector<DevConsoleLine>::const_reverse_iterator lineIter = m_lines.rbegin();

    for( lineIter; lineIter != m_lines.rend(); lineIter++ ) {
        // Is there room to print it?
        if( cameraBounds.GetDimensions().y < lineHeight ) {
            break;
        }

        // Is the channel active?
        if( (m_activeChannels & lineIter->m_channel) == 0 ) {
            continue;
        }

        AABB2 lineBox = cameraBounds.CarveBoxOffBottom( 0.f, lineHeight );
        std::string lineText = Stringf( "%d, %.02f: %s", lineIter->m_frameNumber, lineIter->m_printTime, lineIter->m_string.c_str() );

        font->AddVeretsForTextInBox2D( textVerts, lineBox, lineHeight, lineText, lineIter->m_color, 1.f, ALIGN_CENTER_LEFT );
    }

    renderer->BindTexture( nullptr );
    renderer->DrawVertexArray( backgroundVerts );

    renderer->BindTexture( font->GetTexture() );
    renderer->DrawVertexArray( textVerts );
}


bool DevConsole::IsTakingInput() const {
    return false; // FIXME: Will eventually want to consume input
}


DevConsoleChannel DevConsole::GetChannelFromString( const std::string& inputString ) const {
    std::string lowerName = StringToLower( inputString );
    DevConsoleChannel channel = CHANNEL_UNDEFINED;
    int channelIndex;

    for( channelIndex = 0; channelIndex < MAX_CHANNELS; channelIndex++ ) {
        const std::string& channelName = m_channels[channelIndex];

        if( channelName == lowerName ) {
            break;
        } else if( channelName == "" ) {
            channelIndex = 999999; // Not found
            break;
        }
    }

    if( channelIndex < MAX_CHANNELS ) {
        channel = 0x01 << channelIndex;
    } else {
        DevConsoleChannel integerChannel = strtoul( inputString.c_str(), NULL, 0 );

        if( integerChannel != 0 ) {
            channel = integerChannel;
        } else {
            channel = CHANNEL_INFO;
        }
    }

    return channel;
}


DevConsoleChannel DevConsole::AddChannelByName( const std::string& inputName, bool enableChannel /*= true */ ) {
    std::string lowerName = StringToLower( inputName );
    int channelIndex = 0;

    for( channelIndex = 0; channelIndex < MAX_CHANNELS; channelIndex++ ) {
        const std::string& channelName = m_channels[channelIndex];
        if( lowerName == channelName ) {
            std::string warningMsg = Stringf( "Warning: Channel name (%s) already exists", inputName.c_str() );
            PrintString( warningMsg.c_str(), CONSOLE_WARNING, CHANNEL_WARNING );
            DevConsoleChannel channel = 0x01 << channelIndex;
            return channel;
        } else if( channelName == "" ) {
            break;
        }
    }

    if( channelIndex == MAX_CHANNELS ) {
        std::string errorMsg = Stringf( "Error: Max number of channels (%d) already exist. Not adding new channel (%s)", MAX_CHANNELS, lowerName.c_str() );
        PrintString( errorMsg.c_str(), CONSOLE_ERROR, CHANNEL_ERROR );
        return CHANNEL_UNDEFINED;
    }

    // Passes all the checks... add to the registry
    m_channels[channelIndex] = lowerName;

    if( enableChannel ) {
        EnableChannel( channelIndex );
    }

    DevConsoleChannel channel = 0x01 << channelIndex;
    return channel;
}


void DevConsole::SwitchToChannel( DevConsoleChannel channel ) {
    m_activeChannels = channel;
}


void DevConsole::SwitchToChannel( const std::string& channelName ) {
    DevConsoleChannel channel = GetChannelFromString( channelName );
    SwitchToChannel( channel );
}


void DevConsole::SetActiveChannels( int numChannels, ... ) {
    Strings channels;
    va_list inputChannels;
    va_start( inputChannels, numChannels );

    for( int inputIndex = 0; inputIndex < numChannels; inputIndex++ ) {
        const std::string& inputName = va_arg( inputChannels, std::string );
        channels.push_back( inputName );
    }

    va_end( inputChannels );

    SetActiveChannels( channels );
}


void DevConsole::SetActiveChannels( const std::string& channelNamesCSV ) {
    Strings channels = SplitStringOnDelimeter( channelNamesCSV, ',' );
    SetActiveChannels( channels );
}


void DevConsole::PrintString( std::string inputString, const Rgba& inputColor /*= Rgba::WHITE */, DevConsoleChannel inputChannel /*= CHANNEL_UNDEFINED */ ) {
    // Try to determine best channel based on color
    if( inputChannel == CHANNEL_UNDEFINED ) {
        if( inputColor == CONSOLE_COMMAND ) {
            inputChannel = CHANNEL_COMMAND;
        } else if( inputColor == CONSOLE_INFO ) {
            inputChannel = CHANNEL_INFO;
        } else if( inputColor == CONSOLE_WARNING ) {
            inputChannel = CHANNEL_WARNING;
        } else if( inputColor == CONSOLE_ERROR ) {
            inputChannel = CHANNEL_ERROR;
        } else {
            inputChannel = CHANNEL_INFO;
        }
    }

    m_lines.emplace_back( inputString, m_frameNumber, inputColor, inputChannel );
}


void DevConsole::PrintString( std::string inputString, const Rgba& inputColor, const std::string& inputChannelName ) {
    DevConsoleChannel channel = GetChannelFromString( inputChannelName );
    PrintString( inputString, inputColor, channel );
}


void DevConsole::ExecuteCommandString( const std::string& commandString ) {
    Strings commandList = SplitStringOnDelimeter( commandString, '\n' );
    int numCommands = (int)commandList.size();

    for( int commandIndex = 0; commandIndex < numCommands; commandIndex++ ) {
        const std::string& command = commandList[commandIndex];
        ExecuteCommandLine( command );
    }
}


void DevConsole::Toggle() {
    m_mode = (DevConsoleMode)(1 - m_mode);
}


void DevConsole::RunCommandTests() {
    ExecuteCommandString( "BadCommand" ); // Should fail

    ExecuteCommandString( "Help" );       // Print HELP

    ExecuteCommandString( "Test badArgs = badFormat" ); // Should fail
    ExecuteCommandString( "Test goodArgs=goodFormat" ); // Call test function, print args

    ExecuteCommandString( "Test multiple=lines\nExec file=Data/Gameplay/TestExecFile.xml" ); // Print test with args, then print exec
}


void DevConsole::ExecuteCommandLine( const std::string& commandString ) {
    Strings args = SplitStringOnDelimeter( commandString, ' ' );
    std::string command = args[0];

    if( command != "Echo" ) {
        PrintString( Stringf( "> %s", commandString.c_str() ), CONSOLE_COMMAND );
    }

    // Check Valid Commands
    Strings validCommands;
    g_theEventSystem->GetSubscribedEventsList( validCommands );

    bool commandFound = false;
    int numValidCommands = (int)validCommands.size();

    for( int commandIndex = 0; commandIndex < numValidCommands; commandIndex++ ) {
        if( command == validCommands[commandIndex] ) {
            commandFound = true;
            break;
        }
    }

    if( !commandFound ) {
        PrintString( Stringf( "ERROR: Invalid Command (%s)", command.c_str() ), CONSOLE_ERROR );
        PrintString(          "     - Type HELP for a list of available commands", CONSOLE_ERROR );
        return;
    }

    // Parse Arguments
    EventArgs eventArgs = EventArgs();
    int numArgs = (int)args.size();

    for( int argIndex = 1; argIndex < numArgs; argIndex++ ) {
        std::string keyValueString = args[argIndex];
        Strings splitKeyValue = SplitStringOnDelimeter( keyValueString, '=' );
        int numParts = (int)splitKeyValue.size();

        if( numParts != 2 ) {
            PrintString( Stringf( "ERROR: Invalid Argument Format (%s)", keyValueString.c_str() ), CONSOLE_ERROR );
            PrintString( Stringf( "     - Usage Example: %s key1=value1 key2=value2", command.c_str() ), CONSOLE_ERROR );
            return;
        }

        eventArgs.SetValue( splitKeyValue[0], splitKeyValue[1] );
    }

    // Finally Fire the Event
    g_theEventSystem->FireEvent( command, eventArgs );
}


void DevConsole::SetActiveChannels( const Strings& channels ) {
    int numChannels = (int)channels.size();

    for( int inputIndex = 0; inputIndex < numChannels; inputIndex++ ) {
        std::string inputName = channels[inputIndex];
        bool clearChannel = false;

        if( inputName[0] == '!' ) {
            clearChannel = true;
            inputName = inputName.substr( 1 );
        }

        inputName = StringToLower( inputName );

        for( int channelIndex = 0; channelIndex < MAX_CHANNELS; channelIndex++ ) {
            const std::string& channelName = m_channels[channelIndex];

            if( channelName == inputName ) {
                if( clearChannel ) {
                    DisableChannel( channelIndex );
                } else {
                    EnableChannel( channelIndex );
                }

                break;
            } else if( channelName == "" ) {
                const char* enableText = clearChannel ? "" : " and enabling";
                std::string warningMsg = Stringf( "Warning: Specified channel (%s) does not exist.  Adding%s it for you.", inputName.c_str(), enableText );
                PrintString( warningMsg.c_str(), CONSOLE_WARNING, CHANNEL_WARNING );

                AddChannelByName( inputName, !clearChannel );
                break;
            }
        }
    }
}


void DevConsole::EnableChannel( int channelIndex ) {
    DevConsoleChannel channelBitmask = 0x01 << channelIndex;
    m_activeChannels |= channelBitmask;
}


void DevConsole::DisableChannel( int channelIndex ) {
    DevConsoleChannel channelBitmask = 0x01 << channelIndex;
    m_activeChannels &= ~channelBitmask;
}
