#include "AnimationDefinition.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Framework/LoggerSubsystem.hpp"
std::vector<AnimationDefinition> AnimationDefinition::s_definitions = {};

void AnimationDefinition::LoadDefinitions(const char* path)
{
    LOG(LogResource, Info, "AnimationDefinition::LoadDefinitions    %s", "Start Loading AnimationDefinition\n");
    XmlDocument animDefinitions;
    XmlResult   result = animDefinitions.LoadFile(path);
    if (result == XmlResult::XML_SUCCESS)
    {
        XmlElement* rootElement = animDefinitions.RootElement();
        if (rootElement)
        {
            printf("AnimationDefinition::LoadDefinitions    AnimationDefinition from \"%s\" was loaded\n", path);
            const XmlElement* element = rootElement->FirstChildElement();
            while (element != nullptr)
            {
                auto animDef = AnimationDefinition(*element);
                s_definitions.push_back(animDef);
                element = element->NextSiblingElement();
            }
        }
        else
        {
            printf("AnimationDefinition::LoadDefinitions    AnimationDefinition from \"%s\"was invalid (missing root element)\n", path);
        }
    }
    else
    {
        printf("AnimationDefinition::LoadDefinitions    Failed to load AnimationDefinition from \"%s\"\n", path);
    }
}

void AnimationDefinition::ClearDefinitions()
{
    for (AnimationDefinition definition : s_definitions)
    {
    }
}

AnimationDefinition* AnimationDefinition::GetByName(const std::string& name)
{
    for (int i = 0; i < static_cast<int>(s_definitions.size()); i++)
    {
        if (s_definitions[i].m_name == name)
        {
            return &s_definitions[i];
        }
    }
    return nullptr;
}

const SpriteAnimDefinition* AnimationDefinition::GetAnimationDefinition()
{
    return m_animDef;
}

int AnimationDefinition::GetAnimationTotalFrame()
{
    return m_endFrame - m_startFrame + 1;
}

float AnimationDefinition::GetAnimationLength()
{
    return m_animDef->GetDuration();
}

SpriteAnimPlaybackType AnimationDefinition::GetAnimationPlaybackType()
{
    return m_playbackMode;
}

SpriteSheet* AnimationDefinition::GetAnimationSpriteSheet()
{
    return m_spriteSheet;
}

Vec2 AnimationDefinition::GetSpriteWorldDimension()
{
    return m_spriteWorldDimensions;
}

bool AnimationDefinition::GetRestoreSpriteDimension()
{
    return m_restoreSpriteDimensions;
}

AnimationDefinition::AnimationDefinition(const XmlElement& animDefElement)
{
    m_name                   = ParseXmlAttribute(animDefElement, "name", m_name);
    m_cellCount              = ParseXmlAttribute(animDefElement, "cellCount", m_cellCount);
    m_scaleBySpeed           = ParseXmlAttribute(animDefElement, "scaleBySpeed", m_scaleBySpeed);
    m_secondsPerFrame        = ParseXmlAttribute(animDefElement, "secondsPerFrame", m_secondsPerFrame);
    std::string playbackMode = "Loop";
    playbackMode             = ParseXmlAttribute(animDefElement, "playbackMode", playbackMode);
    if (playbackMode == "Loop")
    {
        m_playbackMode = SpriteAnimPlaybackType::LOOP;
    }
    if (playbackMode == "Once")
    {
        m_playbackMode = SpriteAnimPlaybackType::ONCE;
    }
    if (playbackMode == "Pingpong")
    {
        m_playbackMode = SpriteAnimPlaybackType::PING_PONG;
    }
    int startFrame            = ParseXmlAttribute(animDefElement, "startFrame", 0);
    int endFrame              = ParseXmlAttribute(animDefElement, "endFrame", 0);
    m_spriteSheet             = new SpriteSheet(*g_theRenderer->CreateOrGetTextureFromFile(ParseXmlAttribute(animDefElement, "spriteSheet", m_name).c_str()), m_cellCount);
    m_animDef                 = new SpriteAnimDefinition(*m_spriteSheet, startFrame, endFrame, 1.0f / m_secondsPerFrame, m_playbackMode);
    m_spriteWorldDimensions   = ParseXmlAttribute(animDefElement, "spriteWorldDimensions", m_spriteWorldDimensions);
    m_restoreSpriteDimensions = ParseXmlAttribute(animDefElement, "restoreSpriteDimensions", m_restoreSpriteDimensions);
    printf("AnimationDefinition::AnimationDefinition    + Load Definition \"%s\" \n", m_name.c_str());
}
