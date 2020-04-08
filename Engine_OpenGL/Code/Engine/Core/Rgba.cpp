#include "Engine/Core/Rgba.hpp"


const Rgba Rgba::BLACK   = Rgba( 0.f, 0.f, 0.f, 1.f );
const Rgba Rgba::BLUE    = Rgba( 0.09f, 0.09f, 0.87f, 1.f );
const Rgba Rgba::CYAN    = Rgba( 0.f, 1.f, 1.f, 1.f );
const Rgba Rgba::GRAY    = Rgba( 0.2f, 0.2f, 0.2f, 1.f );
const Rgba Rgba::GREEN   = Rgba( 0.f, 0.75f, 0.f, 1.f );
const Rgba Rgba::MAGENTA = Rgba( 1.f, 0.f, 1.f, 1.f );
const Rgba Rgba::PURPLE  = Rgba( 0.75f, 0.09f, 0.88f, 1.f );
const Rgba Rgba::RED     = Rgba( 1.f, 0.f, 0.f, 1.f );
const Rgba Rgba::WHITE   = Rgba( 1.f, 1.f, 1.f, 1.f );
const Rgba Rgba::YELLOW  = Rgba( 1.f, 1.f, 0.f, 1.f );

const std::map<std::string, const Rgba&> Rgba::s_colors = {
    { "BLACk",   Rgba::BLACK   },
    { "BLUE",    Rgba::BLUE    },
    { "CYAN",    Rgba::CYAN    },
    { "GRAY",    Rgba::GRAY    },
    { "GREEN",   Rgba::GREEN   },
    { "MAGENTA", Rgba::MAGENTA },
    { "PURPLE",  Rgba::PURPLE  },
    { "RED",     Rgba::RED     },
    { "WHITE",   Rgba::WHITE   },
    { "YELLOW",  Rgba::YELLOW  },
};


Rgba::Rgba() {

}


Rgba::Rgba( float r, float g, float b, float a/*=1.f */ ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}


Rgba::Rgba( const char* initialValues ) {
    SetFromText( initialValues );
}


bool Rgba::operator==( const Rgba& compare ) const {
    return (compare.r == r && compare.g == g && compare.b == b && compare.a == a);
}


bool Rgba::operator!=( const Rgba& compare ) const {
    return !operator==( compare );
}


const std::string Rgba::GetAsString() const {
    if( a == 1.f ) {
        return Stringf( "%.02f,%.02f,%.02f", r, g, b );
    } else {
        return Stringf( "%.02f,%.02f,%.02f,%.02f", r, g, b, a );
    }
}


void Rgba::SetFromText( const char* valuesAsText ) {
    std::vector<std::string> splitString = SplitStringOnDelimeter( valuesAsText, ',' );
    int numValues = (int)splitString.size();
    GUARANTEE_OR_DIE( numValues == 1 || numValues == 3 || numValues == 4, Stringf( "Invalid initialization string (%s) for Rgba", valuesAsText ) );

    // Try setting from color name first
    if( numValues == 1 ) {
        std::map<std::string, const Rgba&>::const_iterator colorIter = s_colors.find( valuesAsText );

        GUARANTEE_OR_DIE( colorIter != s_colors.end(), Stringf( "Color name (%s) not found for Rgba", valuesAsText ) );

        *this = colorIter->second;
        return;
    }

    // Try setting from numerical values second
    r = (float)atof( splitString[0].c_str() ) / 255.f;
    g = (float)atof( splitString[1].c_str() ) / 255.f;
    b = (float)atof( splitString[2].c_str() ) / 255.f;

    if( numValues == 3 ) {
        a = 1.0f;
    } else {
        a = (float)atof( splitString[3].c_str() ) / 255.f;
    }
}


void Rgba::SetFromBytes( float redByte, float greenByte, float blueByte, float alphaByte /*= 255 */ ) {
    r = redByte   / 255.f;
    g = greenByte / 255.f;
    b = blueByte  / 255.f;
    a = alphaByte / 255.f;
}
