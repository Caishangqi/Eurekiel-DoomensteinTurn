﻿#include "Animation.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"

Animation::Animation(const XmlElement& animationElement)
{
    m_name            = ParseXmlAttribute(animationElement, "name", m_name);
    m_cellCount       = ParseXmlAttribute(animationElement, "cellCount", m_cellCount);
    m_shader          = g_theRenderer->CreateShaderFromFile(ParseXmlAttribute(animationElement, "shader", m_name).c_str(), VertexType::Vertex_PCU);
    m_spriteSheet     = new SpriteSheet(*g_theRenderer->CreateOrGetTextureFromFile(ParseXmlAttribute(animationElement, "spriteSheet", m_name).c_str()), m_cellCount);
    m_startFrame      = ParseXmlAttribute(animationElement, "startFrame", 0);
    m_endFrame        = ParseXmlAttribute(animationElement, "endFrame", 0);
    m_secondsPerFrame = ParseXmlAttribute(animationElement, "secondsPerFrame", m_secondsPerFrame);
    m_spriteAnim      = new SpriteAnimDefinition(*m_spriteSheet, m_startFrame, m_endFrame, 1.0f / m_secondsPerFrame, m_playbackType);
    printf("Animation::Animation    Create Animation: %s \n", m_name.c_str());
}

Animation::~Animation()
{
    POINTER_SAFE_DELETE(m_shader)
}

float Animation::GetAnimationLength()
{
    return m_spriteAnim->GetDuration();
}

const SpriteAnimDefinition* Animation::GetAnimationDefinition()
{
    return m_spriteAnim;
}

int Animation::GetAnimationTotalFrame()
{
    return m_endFrame - m_startFrame + 1;
}

SpriteAnimPlaybackType Animation::GetAnimationPlaybackType()
{
    return m_playbackType;
}
