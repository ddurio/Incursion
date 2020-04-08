#include "Engine/Core/NamedStrings.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"



NamedStrings::NamedStrings() {
}


NamedStrings::NamedStrings( const XMLElement& element ) {
    SetFromXMLElementAttributes( element );
}


void NamedStrings::SetFromXMLElementAttributes( const XMLElement& element ) {
    for( const XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next() ) {
        SetValue( attribute->Name(), attribute->Value() );
    }
}


void NamedStrings::SetValue( std::string key, std::string value ) {
    std::map<std::string, std::string>::iterator valueIter = m_keyValuePairs.find( key );

    if( valueIter == m_keyValuePairs.end() ) {
        m_keyValuePairs.insert( { key, value } );
    } else {
        m_keyValuePairs[key] = value;
    }
}


bool NamedStrings::GetValue( const std::string& key, bool defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return StringToBool( valueIter->second );
    }
}


int NamedStrings::GetValue( const std::string& key, int defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return atoi( valueIter->second.c_str() );
    }
}


float NamedStrings::GetValue( const std::string& key, float defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return (float)atof( valueIter->second.c_str() );
    }
}


std::string NamedStrings::GetValue( const std::string& key, std::string defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return valueIter->second;
    }
}


std::string NamedStrings::GetValue( const std::string& key, const char* defaultValue ) const {
    return GetValue( key, std::string( defaultValue ) );
}


Rgba NamedStrings::GetValue( const std::string& key, Rgba defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return Rgba( valueIter->second.c_str() );
    }
}


Vec2 NamedStrings::GetValue( const std::string& key, Vec2 defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return Vec2( valueIter->second.c_str() );
    }
}


Vec3 NamedStrings::GetValue( const std::string& key, Vec3 defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return Vec3( valueIter->second.c_str() );
    }
}


IntVec2 NamedStrings::GetValue( const std::string& key, IntVec2 defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return IntVec2( valueIter->second.c_str() );
    }

}


IntRange NamedStrings::GetValue( const std::string& key, IntRange defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return IntRange( valueIter->second.c_str() );
    }

}


FloatRange NamedStrings::GetValue( const std::string& key, FloatRange defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return FloatRange( valueIter->second.c_str() );
    }
}


DevConsoleChannel NamedStrings::GetValue( const std::string& key, DevConsoleChannel defaultValue ) const {
    std::map<std::string, std::string>::const_iterator valueIter = m_keyValuePairs.find( key );
    
    if( valueIter == m_keyValuePairs.end() ) {
        return defaultValue;
    } else {
        return g_theDevConsole->GetChannelFromString( valueIter->second );
        //return strtoul( valueIter->second.c_str() );
    }
}


std::string NamedStrings::GetAsString() const {
    std::string argsAsString = "";
    std::map<std::string, std::string>::const_iterator pairsIter = m_keyValuePairs.begin();

    // Must seed with first value to avoid extra spaces
    std::string firstKey = pairsIter->first;
    std::string firstValue = pairsIter->second;
    argsAsString = Stringf( "%s=%s", firstKey.c_str(), firstValue.c_str() );
    pairsIter++;

    for( pairsIter; pairsIter != m_keyValuePairs.end(); pairsIter++ ) {
        std::string key = pairsIter->first;
        std::string value = pairsIter->second;
        argsAsString = Stringf( "%s %s=%s", argsAsString.c_str(), key.c_str(), value.c_str() );
    }

    return argsAsString;
}
