#pragma once
#include "ITurnState.hpp"
#include "Engine/Core/Timer.hpp"

class Character;

class StateCharacterLeave : public ITurnState
{
public:
    StateCharacterLeave(Character* character);
    ~StateCharacterLeave() override;

    void OnInit() override;
    void OnEnter() override;
    void Update(float dt) override;
    void Exit() override;

private:
    Character* m_character = nullptr;
    Timer*     m_timer     = nullptr;
};
