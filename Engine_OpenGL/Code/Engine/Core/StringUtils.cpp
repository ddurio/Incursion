#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <cctype>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... ) {
	char textLiteral[STRINGF_STACK_LOCAL_TEMP_LENGTH];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[STRINGF_STACK_LOCAL_TEMP_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... ) {
	char textLiteralSmall[STRINGF_STACK_LOCAL_TEMP_LENGTH];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[maxLength];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[maxLength - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}


const std::vector<std::string> SplitStringOnDelimeter( const std::string& originalString, char delimeter ) {
    std::string stringToSplit = originalString;
    std::vector<std::string> splitString;
    int endIndex = (int)stringToSplit.find_first_of( delimeter );

    while( endIndex != std::string::npos ) {
        splitString.emplace_back( stringToSplit, 0, endIndex );

        stringToSplit = stringToSplit.substr( endIndex + 1 );
        endIndex = (int)stringToSplit.find_first_of( delimeter );
    }

    splitString.emplace_back( stringToSplit );
    return splitString;
}


bool StringToBool( const std::string inString ) {
    if( inString == "0" ||
        inString == "f" ||
        inString == "false" ||
        inString == "FALSE" ) {
        return false;
    } else if( inString == "1" ||
        inString == "t" ||
        inString == "true" ||
        inString == "TRUE" ) {
        return true;
    } else {
        return false;
    }
}


bool TextToBool( const char* inText ) {
    return StringToBool( inText );
}


const std::string StringToLower( const std::string& inputString ) {
    std::string outputString = inputString;
    std::string::iterator strIter;

    for( strIter = outputString.begin(); strIter != outputString.end(); strIter++ ) {
        char& letter = *strIter;
        letter = (char)std::tolower( letter );
    }

    return outputString;
}

