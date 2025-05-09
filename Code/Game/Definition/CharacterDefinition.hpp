#pragma once
#include <map>
#include <vector>

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"

class ISkill;
class AnimationDefinition;
class Shader;

class CharacterDefinition
{
public:
    static std::vector<CharacterDefinition> s_definitions;
    static void                             LoadDefinitions(const char* path);
    static void                             ClearDefinitions();
    static CharacterDefinition*             GetByName(const std::string& name);

    CharacterDefinition(const XmlElement& characterDefElement);
    AnimationDefinition* GetAnimationByState(std::string state);

    std::string m_name           = "Default";
    std::string m_faction        = "NEUTRAL";
    float       m_health         = 1.0f;
    float       m_maxMana        = 1.f;
    bool        m_canBePossessed = false;
    float       m_corpseLifetime = 0.0f;
    bool        m_visible        = false;
    bool        m_dieOnSpawn     = false;

    /// Attribute
    int m_baseSpeed        = 0;
    int m_baseShieldPoint  = 0;
    int m_basePhysicAttack = 0;

    /// Collision
    float      m_physicsRadius      = 0.0f;
    float      m_physicsHeight      = 0.0f;
    bool       m_collidesWithWorld  = false;
    bool       m_collidesWithActors = false;
    bool       m_dieOnCollide       = false;
    FloatRange m_damageOnCollide;
    float      m_impulseOnCollied = 0.f;

    /// Physics
    bool  m_simulated = true;
    bool  m_flying    = false;
    float m_walkSpeed = 0.f;
    float m_runSpeed  = 0.0f;
    float m_turnSpeed = 0.0f;
    float m_drag      = 0.0f;

    /// Visual
    Vec2        m_size;
    Vec2        m_pivot;
    std::string m_billboardType = "None";
    bool        m_renderLit     = false;
    bool        m_renderRounded = false;
    Shader*     m_shader        = nullptr;

    /// Skills (Preload)
    std::vector<ISkill*> m_skills;

    /// Animations
    AnimationDefinition*                        m_defaultAnimation = nullptr;
    std::map<std::string, AnimationDefinition*> m_animations; // pre registered
};
