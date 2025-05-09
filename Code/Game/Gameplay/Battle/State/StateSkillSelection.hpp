#pragma once
#include "ITurnState.hpp"

class WidgetSkillSelection;
class Character;

class StateSkillSelection : public ITurnState
{
public:
    StateSkillSelection(Character* character);
    ~StateSkillSelection() override;

    void OnEnter() override;
    void OnInit() override;
    void Update(float dt) override;
    void Exit() override;
    bool GetIsFinished() const override;

    Character* GetCharacter() const;

private:
    Character*            m_character            = nullptr;
    WidgetSkillSelection* m_widgetSkillSelection = nullptr;
};
