#pragma once
#include <string>
#include <vector>

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/Definition/AnimationDefinition.hpp"
#include "Game/Gameplay/Battle/Buff/Buff.hpp"

class Animation;
class Character;

struct DamageData
{
    DamageData()
    {
    };

    void AddBuff(Buff b)
    {
        m_buffs.push_back(b);
    }

    Character*        m_instigator = nullptr;
    int               m_damage     = 0;
    int               m_shield     = 0;
    std::vector<Buff> m_buffs;
};

class ISkill
{
public:
    virtual bool StartAction(Character* instigator);
    virtual void StopAction(Character* instigator);

    /// Getter
    virtual bool                          GetActionIsStarted();
    virtual bool                          GetCanStart();
    virtual Character*                    GetOwner();
    virtual const std::vector<Character*> GetTargets();
    virtual const std::vector<Character*> GetPotentialTargets();
    virtual AnimationDefinition*          GetStartAnimation();
    virtual std::string                   GetStartAnimationName();
    virtual bool                          GetMoveToTarget();
    virtual Vec3                          GetMoveToOffset();

    ISkill()          = default;
    virtual ~ISkill() = default;

    ISkill(const ISkill& other)
        : m_name(other.m_name),
          m_alias(other.m_alias),
          m_description(other.m_description),
          m_owner(other.m_owner),
          m_maxTargets(other.m_maxTargets),
          m_scope(other.m_scope),
          m_bIsStarted(other.m_bIsStarted),
          m_targets(other.m_targets),
          m_moveToTarget(other.m_moveToTarget),
          m_moveToOffSet(other.m_moveToOffSet),
          m_tags(other.m_tags)
    {
    }

    ISkill(ISkill&& other) noexcept
        : m_name(std::move(other.m_name)),
          m_alias(std::move(other.m_alias)),
          m_description(std::move(other.m_description)),
          m_owner(other.m_owner),
          m_maxTargets(other.m_maxTargets),
          m_scope(std::move(other.m_scope)),
          m_bIsStarted(other.m_bIsStarted),
          m_targets(std::move(other.m_targets)),
          m_moveToTarget(other.m_moveToTarget),
          m_moveToOffSet(std::move(other.m_moveToOffSet)),
          m_tags(std::move(other.m_tags))
    {
    }

    ISkill& operator=(const ISkill& other)
    {
        if (this == &other)
            return *this;
        m_name         = other.m_name;
        m_alias        = other.m_alias;
        m_description  = other.m_description;
        m_owner        = other.m_owner;
        m_maxTargets   = other.m_maxTargets;
        m_scope        = other.m_scope;
        m_bIsStarted   = other.m_bIsStarted;
        m_targets      = other.m_targets;
        m_moveToTarget = other.m_moveToTarget;
        m_moveToOffSet = other.m_moveToOffSet;
        m_tags         = other.m_tags;
        return *this;
    }

    ISkill& operator=(ISkill&& other) noexcept
    {
        if (this == &other)
            return *this;
        m_name         = std::move(other.m_name);
        m_alias        = std::move(other.m_alias);
        m_description  = std::move(other.m_description);
        m_owner        = other.m_owner;
        m_maxTargets   = other.m_maxTargets;
        m_scope        = std::move(other.m_scope);
        m_bIsStarted   = other.m_bIsStarted;
        m_targets      = std::move(other.m_targets);
        m_moveToTarget = other.m_moveToTarget;
        m_moveToOffSet = std::move(other.m_moveToOffSet);
        m_tags         = std::move(other.m_tags);
        return *this;
    }

    /// Setter
    virtual Character* SetOwner(Character* inOwner);


    /// ZSGC
    virtual void       ParseDataFromXML(const XmlElement& skillDataElement);
    virtual Character* AddToTarget(Character* inTarget);
    virtual Character* RemoveFromTarget(Character* inTarget);
    virtual int        ClearTargets();
    virtual ISkill*    Duplicate();

    std::string m_name;
    std::string m_alias; // Alias that display on GUI
    std::string m_description; // Description that display on GUI

protected:
    Character*              m_owner      = nullptr;
    int                     m_maxTargets = 1;
    std::string             m_scope      = "Friend"; // Friend , Enemy, Both
    bool                    m_bIsStarted = false;
    std::vector<Character*> m_targets;
    bool                    m_moveToTarget = false;
    Vec3                    m_moveToOffSet = Vec3::ZERO;
    Strings                 m_tags;
};
