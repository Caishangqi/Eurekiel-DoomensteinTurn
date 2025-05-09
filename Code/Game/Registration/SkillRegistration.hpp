#pragma once
#include <string>
#include <vector>

class ISkill;

/// We do not register SKill from xml because skill is highly code base
/// We register skills in this class and expose an skill builder method
class SkillRegistration
{
public:
    SkillRegistration()                         = delete;
    SkillRegistration(const SkillRegistration&) = delete;

    static void          LoadRegistrations();
    static void          ClearRegistrations();
    static const ISkill* GetSkillByName(std::string name);
    static ISkill*       GetCopiedSkillByName(std::string name);

protected:
    static void LoadRegistration(ISkill* inSkill);

private:
    static std::vector<ISkill*> s_registeredSkills;
};
