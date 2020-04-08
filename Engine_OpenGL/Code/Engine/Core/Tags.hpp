#pragma once
#include "Game/GameCommon.hpp"


class Tags {
    public:
    void SetTags( std::string tagsCSV );
    void ClearTags( std::string tagsCSV );
    bool HasTags( std::string tagsCSV ) const;

    private:
    Strings m_tags;
};
