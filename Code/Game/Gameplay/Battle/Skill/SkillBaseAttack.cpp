#include "SkillBaseAttack.hpp"

#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/Scene.hpp"
#include "Game/Gameplay/Battle/State/StateMoveToSlot.hpp"

SkillBaseAttack::SkillBaseAttack()
{
    m_name = "SkillBaseAttack";
    m_tags.push_back("Damage");
}

SkillBaseAttack::~SkillBaseAttack()
{
}

bool SkillBaseAttack::StartAction(Character* instigator)
{
    ISkill::StartAction(instigator);
    //----------------------------------------------------------
    // 0) 调用父类收集公共信息（消耗 SP / CD 等）
    //----------------------------------------------------------

    //----------------------------------------------------------
    // 1) 组装潜在目标列表
    //----------------------------------------------------------
    std::vector<Character*> potential = GetPotentialTargets(); // 按阵营过滤
    if (potential.empty())
        return false; // 没有合法目标

    //----------------------------------------------------------
    // 2) 生成最终 victims 列表
    //    - m_targets[] 是 UI / AI 选中的“首要目标”
    //    - 如果 maxTargets > 1，则从 potential 中随机补足
    //----------------------------------------------------------
    std::vector<Character*> victims;

    // 先把玩家已点选的目标放进去（通常最多 1 个）
    for (Character* c : m_targets)
    {
        if (c && !c->GetIsDead())
            victims.push_back(c);
    }

    // 若需要额外目标
    while (static_cast<int>(victims.size()) < m_maxTargets && !potential.empty())
    {
        // 过滤掉已入选的
        auto it = std::remove_if(potential.begin(), potential.end(),
                                 [&victims](Character* cand)
                                 {
                                     return std::find(victims.begin(), victims.end(), cand) != victims.end();
                                 });
        potential.erase(it, potential.end());
        if (potential.empty()) break;

        // 从剩余潜在目标里随机挑一个
        int idx = g_rng->RollRandomIntInRange(0, static_cast<int>(potential.size()) - 1);
        victims.push_back(potential[idx]);
        potential.erase(potential.begin() + idx); // 避免重复
    }


    for (Character* target : victims)
    {
        if (!target) continue;

        DamageData dmg;
        dmg.m_instigator = instigator;
        dmg.m_damage     = static_cast<int>(m_physicAttackMultiplier *
            static_cast<float>(instigator->GetCharacterDefinition()->m_basePhysicAttack));

        target->ReceiveDamage(dmg);
    }

    return true;
}

void SkillBaseAttack::StopAction(Character* instigator)
{
    UNUSED(instigator)
}

bool SkillBaseAttack::GetCanStart()
{
    return false;
}

const std::vector<Character*> SkillBaseAttack::GetPotentialTargets()
{
    return g_theGame->m_turnBaseSubsystem->GetCurrentScene()->GetCharactersByFraction(m_scope);
}

ISkill* SkillBaseAttack::Duplicate()
{
    return new SkillBaseAttack(*this);
}

AnimationDefinition* SkillBaseAttack::GetStartAnimation()
{
    if (m_maxTargets == 1)
        return m_owner->GetCharacterDefinition()->GetAnimationByState("Attack");
    return m_owner->GetCharacterDefinition()->GetAnimationByState("Attack_AOE");
}

std::string SkillBaseAttack::GetStartAnimationName()
{
    if (m_maxTargets == 1)
        return "Attack";
    return "Attack_AOE";
}

void SkillBaseAttack::ParseDataFromXML(const XmlElement& skillDataElement)
{
    ISkill::ParseDataFromXML(skillDataElement);
    m_animationName = ParseXmlAttribute(skillDataElement, "animation", m_animationName);
}
