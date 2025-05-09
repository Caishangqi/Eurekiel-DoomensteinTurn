#include "SkillRegistration.hpp"
#include "Game/Gameplay/Battle/Skill/ISkill.hpp"
#include "Game/Gameplay/Battle/Skill/SkillBaseAttack.hpp"
std::vector<ISkill*> SkillRegistration::s_registeredSkills = {};

void SkillRegistration::LoadRegistrations()
{
    printf("SkillRegistration::LoadRegistrations    %s", "Start Register Skills\n");
    LoadRegistration(new SkillBaseAttack());
}

void SkillRegistration::ClearRegistrations()
{
}

const ISkill* SkillRegistration::GetSkillByName(std::string name)
{
    for (ISkill* skill : s_registeredSkills)
    {
        if (skill->m_name == name)
        {
            return skill;
        }
    }
    return nullptr;
}

ISkill* SkillRegistration::GetCopiedSkillByName(std::string name)
{
    for (ISkill* skill : s_registeredSkills)
    {
        if (skill->m_name == name)
        {
            return skill->Duplicate();
        }
    }
    return nullptr;
}

void SkillRegistration::LoadRegistration(ISkill* inSkill)
{
    s_registeredSkills.push_back(inSkill);
    printf("SkillRegistration::LoadRegistration     Adding Skill Template [%s] instance\n", inSkill->m_name.c_str());
}
