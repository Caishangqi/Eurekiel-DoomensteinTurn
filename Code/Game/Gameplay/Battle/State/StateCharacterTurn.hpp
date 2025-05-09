#pragma once
#include "ITurnState.hpp"
#include "Game/Gameplay/Character.hpp"

class StateCharacterTurn : public ITurnState
{
public:
    StateCharacterTurn(Character* character);
    ~StateCharacterTurn() override;

    void Update(float dt) override;
    bool GetIsFinished() const override;
    void OnInit() override;
    void OnEnter() override;
    void Exit() override;
    bool GetCharacterTurnOver();

private:
    Character* m_character          = nullptr;
    bool       m_bCharacterTurnOver = false;
};
