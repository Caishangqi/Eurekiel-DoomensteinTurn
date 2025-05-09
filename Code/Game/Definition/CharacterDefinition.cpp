#include "CharacterDefinition.hpp"

#include "AnimationDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Framework/LoggerSubsystem.hpp"
#include "Game/Gameplay/Battle/Skill/ISkill.hpp"
#include "Game/Registration/SkillRegistration.hpp"
std::vector<CharacterDefinition> CharacterDefinition::s_definitions = {};

void CharacterDefinition::LoadDefinitions(const char* path)
{
    LOG(LogResource, Info, "CharacterDefinition::LoadDefinitions    %s", "Start Loading CharacterDefinition\n");
    XmlDocument characterDefinitions;
    XmlResult   result = characterDefinitions.LoadFile(path);
    if (result == XmlResult::XML_SUCCESS)
    {
        XmlElement* rootElement = characterDefinitions.RootElement();
        if (rootElement)
        {
            printf("CharacterDefinition::LoadDefinitions    CharacterDefinition from \"%s\" was loaded\n", path);
            const XmlElement* element = rootElement->FirstChildElement();
            while (element != nullptr)
            {
                auto characterDef = CharacterDefinition(*element);
                s_definitions.push_back(characterDef);
                element = element->NextSiblingElement();
            }
        }
        else
        {
            printf("CharacterDefinition::LoadDefinitions    CharacterDefinition from \"%s\"was invalid (missing root element)\n", path);
        }
    }
    else
    {
        printf("CharacterDefinition::LoadDefinitions    Failed to load CharacterDefinition from \"%s\"\n", path);
    }
}

void CharacterDefinition::ClearDefinitions()
{
}

CharacterDefinition* CharacterDefinition::GetByName(const std::string& name)
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

CharacterDefinition::CharacterDefinition(const XmlElement& characterDefElement)
{
    m_name           = ParseXmlAttribute(characterDefElement, "name", m_name);
    m_faction        = ParseXmlAttribute(characterDefElement, "faction", m_faction);
    m_health         = ParseXmlAttribute(characterDefElement, "health", m_health);
    m_canBePossessed = ParseXmlAttribute(characterDefElement, "canBePossessed", m_canBePossessed);
    m_corpseLifetime = ParseXmlAttribute(characterDefElement, "corpseLifetime", m_corpseLifetime);
    m_visible        = ParseXmlAttribute(characterDefElement, "visible", m_visible);
    m_dieOnSpawn     = ParseXmlAttribute(characterDefElement, "dieOnSpawn", m_dieOnSpawn);

    const XmlElement* collisionElement = FindChildElementByName(characterDefElement, "Collision");
    if (collisionElement)
    {
        m_physicsRadius      = ParseXmlAttribute(*collisionElement, "radius", m_physicsRadius);
        m_physicsHeight      = ParseXmlAttribute(*collisionElement, "height", m_physicsHeight);
        m_collidesWithWorld  = ParseXmlAttribute(*collisionElement, "collidesWithWorld", m_collidesWithWorld);
        m_collidesWithActors = ParseXmlAttribute(*collisionElement, "collidesWithActors", m_collidesWithActors);
        m_dieOnCollide       = ParseXmlAttribute(*collisionElement, "dieOnCollide", m_dieOnCollide);
        m_damageOnCollide    = ParseXmlAttribute(*collisionElement, "damageOnCollide", m_damageOnCollide);
        m_impulseOnCollied   = ParseXmlAttribute(*collisionElement, "impulseOnCollide", m_impulseOnCollied);
    }
    const XmlElement* physicsElement = FindChildElementByName(characterDefElement, "Physics");
    if (physicsElement)
    {
        m_simulated = ParseXmlAttribute(*physicsElement, "simulated", m_simulated);
        m_flying    = ParseXmlAttribute(*physicsElement, "flying", m_flying);
        m_walkSpeed = ParseXmlAttribute(*physicsElement, "walkSpeed", m_walkSpeed);
        m_runSpeed  = ParseXmlAttribute(*physicsElement, "runSpeed", m_runSpeed);
        m_turnSpeed = ParseXmlAttribute(*physicsElement, "turnSpeed", m_turnSpeed);
        m_drag      = ParseXmlAttribute(*physicsElement, "drag", m_drag);
    }

    const XmlElement* visualsElement = FindChildElementByName(characterDefElement, "Visuals");
    if (visualsElement)
    {
        m_size          = ParseXmlAttribute(*visualsElement, "size", m_size);
        m_pivot         = ParseXmlAttribute(*visualsElement, "pivot", m_pivot);
        m_billboardType = ParseXmlAttribute(*visualsElement, "billboardType", m_billboardType);
        m_renderLit     = ParseXmlAttribute(*visualsElement, "renderLit", m_renderLit);
        m_renderRounded = ParseXmlAttribute(*visualsElement, "renderRounded", m_renderRounded);
        m_shader        = g_theRenderer->CreateShaderFromFile(ParseXmlAttribute(*visualsElement, "shader", m_name).c_str(), VertexType::Vertex_PCUTBN);
        if (visualsElement->ChildElementCount() > 0) // Load Animations
        {
            const XmlElement* animationElement = FindChildElementByName(*visualsElement, "Animations");
            std::string       defaultAnim      = ParseXmlAttribute(*animationElement, "defaultAnimation", defaultAnim);
            m_defaultAnimation                 = AnimationDefinition::GetByName(defaultAnim);
            if (!m_defaultAnimation)
                ERROR_AND_DIE("CharacterDefinition::CharacterDefinition     Default Character Animation can not found\n")
            const XmlElement* element = animationElement->FirstChildElement();
            while (element != nullptr)
            {
                std::string          keyState     = ParseXmlAttribute(*element, "state", keyState);
                std::string          valueAnim    = ParseXmlAttribute(*element, "animationName", valueAnim);
                AnimationDefinition* valueAnimDef = AnimationDefinition::GetByName(valueAnim);
                if (valueAnimDef)
                {
                    m_animations.insert(std::pair<std::string, AnimationDefinition*>(keyState, valueAnimDef));
                    element = element->NextSiblingElement();
                }
                else
                {
                    ERROR_AND_DIE("CharacterDefinition::CharacterDefinition     Can not find registered Animation\n")
                }
            }
        }
    }
    const XmlElement* attributeElement = FindChildElementByName(characterDefElement, "Attributes");
    if (attributeElement)
    {
        m_baseSpeed        = ParseXmlAttribute(*attributeElement, "baseSpeed", m_baseSpeed);
        m_baseShieldPoint  = ParseXmlAttribute(*attributeElement, "baseShieldPoint", m_baseShieldPoint);
        m_basePhysicAttack = ParseXmlAttribute(*attributeElement, "basePhysicAttack", m_basePhysicAttack);
        m_maxMana          = ParseXmlAttribute(*attributeElement, "maxMana", m_maxMana);
    }

    const XmlElement* skillElement = FindChildElementByName(characterDefElement, "Skills");
    if (skillElement)
    {
        const XmlElement* element = skillElement->FirstChildElement();
        while (element != nullptr)
        {
            std::string skillName = ParseXmlAttribute(*element, "name", skillName);
            ISkill*     skill     = SkillRegistration::GetCopiedSkillByName(skillName);
            if (skill)
            {
                skill->ParseDataFromXML(*element);
                m_skills.push_back(skill);
            }
            element = element->NextSiblingElement();
        }
    }

    printf("CharacterDefinition::CharacterDefinition    + Load Definition \"%s\" \n", m_name.c_str());
}

AnimationDefinition* CharacterDefinition::GetAnimationByState(std::string state)
{
    for (std::pair<const std::string, AnimationDefinition*>& m_animation : m_animations)
    {
        if (m_animation.first == state)
            return m_animation.second;
    }
    return nullptr;
}
