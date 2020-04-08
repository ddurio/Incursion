#pragma once
#include "Engine/Core/EngineCommon.hpp"
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
typedef std::vector<std::string> Strings;

const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

const Strings SplitStringOnDelimeter( const std::string& originalString, char delimeter );

bool StringToBool( const std::string inString );
bool TextToBool( const char* inText );

const std::string StringToLower( const std::string& inputString );
