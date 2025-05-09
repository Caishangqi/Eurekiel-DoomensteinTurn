#pragma once
#include "ITurnState.hpp"
#include "Game/Definition/SceneDefinition.hpp"

class Timer;
class Character;

class StateMoveToSlot : public ITurnState
{
public:
    StateMoveToSlot(Character* character, CharacterSlot characterSlot);
    ~StateMoveToSlot() override;

    void OnInit() override;
    void OnEnter() override;
    void Update(float dt) override;
    void Exit() override;

private:
    Character*    m_character;
    CharacterSlot m_targetSlot;
    Timer*        m_timer = nullptr;
};
