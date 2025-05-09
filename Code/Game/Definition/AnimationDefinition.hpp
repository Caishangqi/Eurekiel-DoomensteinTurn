#pragma once
#include <string>
#include <vector>

#include "Engine/Core/Timer.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class AnimationDefinition
{
public:
    static std::vector<AnimationDefinition> s_definitions;
    static void                             LoadDefinitions(const char* path);
    static void                             ClearDefinitions();
    static AnimationDefinition*             GetByName(const std::string& name);


    const SpriteAnimDefinition* GetAnimationDefinition();
    int                         GetAnimationTotalFrame();
    float                       GetAnimationLength();
    SpriteAnimPlaybackType      GetAnimationPlaybackType();
    SpriteSheet*                GetAnimationSpriteSheet();
    Vec2                        GetSpriteWorldDimension();
    bool                        GetRestoreSpriteDimension();

    AnimationDefinition(const XmlElement& animDefElement);

private:
    std::string            m_name                    = "Default";
    bool                   m_scaleBySpeed            = false;
    float                  m_secondsPerFrame         = 0.f;
    SpriteAnimPlaybackType m_playbackMode            = SpriteAnimPlaybackType::LOOP;
    SpriteSheet*           m_spriteSheet             = nullptr;
    IntVec2                m_cellCount               = IntVec2::ZERO;
    int                    m_startFrame              = 0;
    int                    m_endFrame                = 0;
    Vec2                   m_spriteWorldDimensions   = Vec2::INVALID;
    bool                   m_restoreSpriteDimensions = true;

    SpriteAnimDefinition* m_animDef = nullptr;
};
