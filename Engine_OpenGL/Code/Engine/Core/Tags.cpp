#include "Engine/Core/Tags.hpp"


void Tags::SetTags( std::string tagsCSV ) {
    Strings splitTags = SplitStringOnDelimeter( tagsCSV, ',' );
    Strings::const_iterator newTagsIter = splitTags.begin();

    for( newTagsIter; newTagsIter != splitTags.end(); newTagsIter++ ) {
        if( (*newTagsIter)[0] == '!' ) {
            ClearTags( *newTagsIter );
            continue;
        }


        Strings::const_iterator oldTagsIter = m_tags.begin();
        bool tagFound = false;

        // Don't add multiple versions of the same tag
        for( oldTagsIter; oldTagsIter != m_tags.end(); oldTagsIter++ ) {
            if( *newTagsIter == *oldTagsIter ) {
                tagFound = true;
                break;
            }
        }

        if( !tagFound ) {
            m_tags.push_back( *newTagsIter );
        }
    }
}


void Tags::ClearTags( std::string tagsCSV ) {
    Strings splitTags = SplitStringOnDelimeter( tagsCSV, ',' );
    Strings::const_iterator newTagsIter = splitTags.begin();

    for( newTagsIter; newTagsIter != splitTags.end(); newTagsIter++ ) {
        if( (*newTagsIter)[0] == '!' ) {
            SetTags( *newTagsIter );
            continue;
        }


        Strings::const_iterator oldTagsIter = m_tags.begin();

        for( oldTagsIter; oldTagsIter != m_tags.end(); oldTagsIter++ ) {
            if( *newTagsIter == *oldTagsIter ) {
                m_tags.erase( oldTagsIter );
                break;
            }
        }
    }
}


bool Tags::HasTags( std::string tagsCSV ) const {
    Strings splitTags = SplitStringOnDelimeter( tagsCSV, ',' );
    Strings::const_iterator newTagsIter = splitTags.begin();
    bool hasAllTags = true;

    for( newTagsIter; newTagsIter != splitTags.end(); newTagsIter++ ) {
        // Default changes based on set/cleared tag is desired
        bool hasTag = (*newTagsIter)[0] == '!';

        Strings::const_iterator oldTagsIter = m_tags.begin();

        for( oldTagsIter; oldTagsIter != m_tags.end(); oldTagsIter++ ) {
            if( *newTagsIter == *oldTagsIter ) {
                hasTag = !hasTag;
                break;
            }
        }

        hasAllTags &= hasTag;
    }

    return hasAllTags;
}
