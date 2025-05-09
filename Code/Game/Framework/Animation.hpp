﻿#pragma once
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class SpriteSheet;

class Animation
{
public:
    Animation(const XmlElement& animationElement);
    ~Animation();


    const SpriteAnimDefinition* GetAnimationDefinition();
    int                         GetAnimationTotalFrame();
    float                       GetAnimationLength();
    SpriteAnimPlaybackType      GetAnimationPlaybackType();
    std::string                 m_name;

private:
    IntVec2 m_cellCount;
    float   m_secondsPerFrame = 0.f;
    int     m_startFrame      = 0;
    int     m_endFrame        = 0;

    Shader*                m_shader       = nullptr;
    const SpriteSheet*     m_spriteSheet  = nullptr;
    SpriteAnimDefinition*  m_spriteAnim   = nullptr;
    SpriteAnimPlaybackType m_playbackType = SpriteAnimPlaybackType::ONCE;
};
