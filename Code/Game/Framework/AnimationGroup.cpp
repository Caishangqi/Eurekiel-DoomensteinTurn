﻿#include "AnimationGroup.hpp"

#include "LoggerSubsystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"


AnimationGroup::AnimationGroup(const XmlElement& animationGroupElement, const SpriteSheet& spriteSheet): m_spriteSheet(spriteSheet)
{
    LOG(LogResource, Info, "AnimationGroup::AnimationGroup    %s", "Start Loading AnimationGroup\n");
    //                                 ‖ 
    m_name                   = ParseXmlAttribute(animationGroupElement, "name", m_name);
    m_scaleBySpeed           = ParseXmlAttribute(animationGroupElement, "scaleBySpeed", m_scaleBySpeed);
    m_secondsPerFrame        = ParseXmlAttribute(animationGroupElement, "secondsPerFrame", m_secondsPerFrame);
    std::string playbackMode = "Loop";
    playbackMode             = ParseXmlAttribute(animationGroupElement, "playbackMode", playbackMode);
    if (playbackMode == "Loop")
    {
        m_playbackType = SpriteAnimPlaybackType::LOOP;
    }
    if (playbackMode == "Once")
    {
        m_playbackType = SpriteAnimPlaybackType::ONCE;
    }
    if (playbackMode == "Pingpong")
    {
        m_playbackType = SpriteAnimPlaybackType::PING_PONG;
    }
    if (animationGroupElement.ChildElementCount() > 0)
    {
        const XmlElement* element = animationGroupElement.FirstChildElement();
        while (element != nullptr)
        {
            Vec3              directionVector  = ParseXmlAttribute(*element, "vector", Vec3::ZERO);
            const XmlElement* animationElement = element->FirstChildElement();
            int               startFrame       = ParseXmlAttribute(*animationElement, "startFrame", 0);
            int               endFrame         = ParseXmlAttribute(*animationElement, "endFrame", 0);
            auto              animation        = SpriteAnimDefinition(spriteSheet, startFrame, endFrame, 1.0f / m_secondsPerFrame, m_playbackType);
            m_animations.insert(std::make_pair(directionVector.GetNormalized(), animation)); // Be-careful that 
            element = element->NextSiblingElement();
            printf("                                 ‖ Add Direction (%d, %d, %d) to Animation Group\n", static_cast<int>(directionVector.x), static_cast<int>(directionVector.y),
                   static_cast<int>(directionVector.z));
        }
    }
}

const SpriteAnimDefinition& AnimationGroup::GetSpriteAnimation(Vec3 direction)
{
    Vec3  leastOffset     = direction;
    float directionScalar = -FLT_MAX;
    for (std::pair<const Vec3, SpriteAnimDefinition>& animation : m_animations)
    {
        float scalar = DotProduct3D(direction, animation.first);
        if (scalar > directionScalar)
        {
            directionScalar = scalar;
            leastOffset     = animation.first;
        }
    }
    return m_animations.at(leastOffset);
}

float AnimationGroup::GetAnimationLength()
{
    float totalLength = -1;
    for (std::pair<const Vec3, SpriteAnimDefinition>& animation : m_animations)
    {
        totalLength = animation.second.GetDuration();
        if (totalLength > 0.f)
            return totalLength;
    }
    return -1;
}

int AnimationGroup::GetAnimationTotalFrame()
{
    int totalFrame = -1;
    for (std::pair<const Vec3, SpriteAnimDefinition>& animation : m_animations)
    {
        totalFrame = animation.second.GetTotalFrameInCycle();
        if (totalFrame > 0)
            return totalFrame;
    }
    return -1;
}
