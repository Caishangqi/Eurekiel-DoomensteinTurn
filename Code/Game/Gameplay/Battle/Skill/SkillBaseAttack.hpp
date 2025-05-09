#pragma once
#include "ISkill.hpp"
#include "Engine/Math/Vec3.hpp"

class SkillBaseAttack : public ISkill
{
public:
    SkillBaseAttack();
    ~SkillBaseAttack() override;

    void ParseDataFromXML(const XmlElement& skillDataElement) override;

    bool                          StartAction(Character* instigator) override;
    void                          StopAction(Character* instigator) override;
    bool                          GetCanStart() override;
    const std::vector<Character*> GetPotentialTargets() override;
    ISkill*                       Duplicate() override;

    AnimationDefinition* GetStartAnimation() override;
    std::string          GetStartAnimationName() override;

private:
    float       m_physicAttackMultiplier = 1.0f;
    std::string m_animationName;
};
