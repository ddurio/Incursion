#pragma once
#include "Engine/Core/EngineCommon.hpp"


class SpriteDef;
class SpriteSheet;

enum SpriteAnimPlaybackType {
    SPRITE_ANIM_PLAYBACK_ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4...
    SPRITE_ANIM_PLAYBACK_LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4...
    SPRITE_ANIM_PLAYBACK_PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2...

    NUM_SPRITE_ANIM_PLAYBACK_TYPES
};


class SpriteAnimDef {
    public:
    SpriteAnimDef( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex,
        float durationSeconds, SpriteAnimPlaybackType playbackType = SPRITE_ANIM_PLAYBACK_LOOP );

    const SpriteDef& GetSpriteDefAtTime( float seconds ) const;
    const SpriteSheet& GetSpriteSheet() const;

    private:
    const SpriteSheet&		m_spriteSheet;
    int				m_startSpriteIndex = -1;
    int				m_endSpriteIndex = -1;
    float				m_durationSeconds = 1.f;
    SpriteAnimPlaybackType	m_playbackType = SPRITE_ANIM_PLAYBACK_LOOP;
};
