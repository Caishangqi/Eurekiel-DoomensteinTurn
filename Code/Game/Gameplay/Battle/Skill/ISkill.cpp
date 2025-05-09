#include "ISkill.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Gameplay/Character.hpp"

bool ISkill::StartAction(Character* instigator)
{
    UNUSED(instigator)
    printf("ISkill::StartAction     Start Action with name [%s] instigator [%s]\n", m_alias.c_str(), instigator->GetCharacterDefinition()->m_name.c_str());
    return false;
}


void ISkill::StopAction(Character* instigator)
{
    UNUSED(instigator)
}

bool ISkill::GetActionIsStarted()
{
    return m_bIsStarted;
}

bool ISkill::GetCanStart()
{
    return false;
}

Character* ISkill::GetOwner()
{
    return m_owner;
}

const std::vector<Character*> ISkill::GetTargets()
{
    return m_targets;
}

const std::vector<Character*> ISkill::GetPotentialTargets()
{
    std::vector<Character*> targets;
    return targets;
}

AnimationDefinition* ISkill::GetStartAnimation()
{
    return nullptr;
}

std::string ISkill::GetStartAnimationName()
{
    return "";
}

bool ISkill::GetMoveToTarget()
{
    return m_moveToTarget;
}

Vec3 ISkill::GetMoveToOffset()
{
    return m_moveToOffSet;
}

Character* ISkill::SetOwner(Character* inOwner)
{
    m_owner = inOwner;
    return m_owner;
}

void ISkill::ParseDataFromXML(const XmlElement& skillDataElement)
{
    printf("ISkill::ParseDataFromXML        Parsing Customize Skill with template [%s]\n", m_name.c_str());
    m_alias        = ParseXmlAttribute(skillDataElement, "alias", m_alias);
    m_scope        = ParseXmlAttribute(skillDataElement, "scope", m_scope);
    m_maxTargets   = ParseXmlAttribute(skillDataElement, "maxTargets", m_maxTargets);
    m_moveToTarget = ParseXmlAttribute(skillDataElement, "moveToTarget", m_moveToTarget);
    m_moveToOffSet = ParseXmlAttribute(skillDataElement, "moveToOffSet", m_moveToOffSet);
}

Character* ISkill::AddToTarget(Character* inTarget)
{
    printf("ISkill::AddToTarget     Add target [%s] to skill [%s]\n", inTarget->GetCharacterDefinition()->m_name.c_str(), m_alias.c_str());
    m_targets.push_back(inTarget);
    return m_targets.back();
}

Character* ISkill::RemoveFromTarget(Character* inTarget)
{
    for (Character* target : m_targets)
    {
        if (target == inTarget)
        {
            target = nullptr;
            return inTarget;
        }
    }
    return nullptr;
}

int ISkill::ClearTargets()
{
    int size = static_cast<int>(m_targets.size());
    m_targets.clear();
    return size;
}

ISkill* ISkill::Duplicate()
{
    return new ISkill(*this);
}
