#include "Engine/Core/DevConsoleLine.hpp"

#include "Engine/Core/Time.hpp"


DevConsoleLine::DevConsoleLine( std::string inputString, int frameNumber, Rgba inputColor /*= Rgba::WHITE */, DevConsoleChannel inputChannel /*= CHANNEL_UNDEFINED */ ) :
    m_string(inputString),
    m_frameNumber(frameNumber),
    m_color(inputColor),
    m_channel(inputChannel) {
    m_printTime = GetCurrentTimeSeconds();
    //DebuggerPrintf( Stringf("New line: %s.\n", inputString.c_str()).c_str() );
}
