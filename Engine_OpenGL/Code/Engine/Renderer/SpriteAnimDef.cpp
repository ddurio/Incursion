#include "Engine/Renderer/SpriteAnimDef.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


SpriteAnimDef::SpriteAnimDef( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType /*= SPRITE_ANIM_PLAYBACK_LOOP */ ) :
    m_spriteSheet( sheet ),
    m_startSpriteIndex( startSpriteIndex ),
    m_endSpriteIndex( endSpriteIndex ),
    m_durationSeconds( durationSeconds ),
    m_playbackType( playbackType ) {
    GUARANTEE_OR_DIE( (m_endSpriteIndex - m_startSpriteIndex) >= 0, "Invalid Start/End Sprite Index" );
}


const SpriteDef& SpriteAnimDef::GetSpriteDefAtTime( float seconds ) const {
    int numSprites = m_endSpriteIndex - m_startSpriteIndex + 1;
    float frameDurationSeconds = m_durationSeconds / numSprites;
    int frameIndex = (int)(seconds / frameDurationSeconds);
    int spriteIndex = -1;

    if( m_playbackType == SPRITE_ANIM_PLAYBACK_ONCE ) {
        frameIndex = ClampInt( frameIndex, 0, numSprites - 1 );
        spriteIndex = m_startSpriteIndex + frameIndex;
    } else if( m_playbackType == SPRITE_ANIM_PLAYBACK_LOOP ) {
        frameIndex = frameIndex % numSprites;
        spriteIndex = m_startSpriteIndex + frameIndex;
    } else if( m_playbackType == SPRITE_ANIM_PLAYBACK_PINGPONG ) {
        int numCycleFrames = (numSprites - 2) + numSprites; // Not counting start & end sprites, double it
        frameIndex = frameIndex % numCycleFrames;

        if( frameIndex < numSprites - 1 ) {
            spriteIndex = m_startSpriteIndex + frameIndex;
        } else {
            int reverseIndex = numCycleFrames - frameIndex;
            spriteIndex = m_startSpriteIndex + reverseIndex;
        }
    }

    return m_spriteSheet.GetSpriteDef( spriteIndex );
}


const SpriteSheet& SpriteAnimDef::GetSpriteSheet() const {
    return m_spriteSheet;
}
