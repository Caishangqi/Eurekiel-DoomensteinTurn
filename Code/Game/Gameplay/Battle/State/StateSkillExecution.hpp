#pragma once
#include "ITurnState.hpp"
#include "Engine/Core/Timer.hpp"
#include "Game/Gameplay/Character.hpp"

class ISkill;

class StateSkillExecution : public ITurnState
{
public:
    StateSkillExecution(ISkill* inExecuteSkill, Character* inCharacter);

    void OnInit() override;
    void OnEnter() override;
    void Update(float dt) override;
    void Exit() override;

private:
    ISkill*    m_executeSkill = nullptr;
    Character* m_character    = nullptr;

    Timer*               m_animationTimer      = nullptr;
    AnimationDefinition* m_animationDefinition = nullptr;
    std::string          m_animationName;
    float                m_animationLength = 0.f;
};
