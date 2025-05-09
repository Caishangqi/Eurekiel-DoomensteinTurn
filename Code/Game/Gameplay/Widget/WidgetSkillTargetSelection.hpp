#pragma once
#include "Game/Framework/Widget.hpp"

class Texture;
class WidgetSkillSelection;
class ISkill;
class StateSkillSelection;
class Character;

class WidgetSkillTargetSelection : public Widget
{
public:
    WidgetSkillTargetSelection(Character* inCharacter, WidgetSkillSelection* inSkillSelectionWidget);
    ~WidgetSkillTargetSelection() override;

    void Draw() const override;
    void Update() override;

    void UpdateKeyInput();
    void UpdateSelectSkill(ISkill* inSkill);
    bool GetIsSelectSkill() const;

private:
    Character*              m_character            = nullptr;
    ISkill*                 m_selectedSkill        = nullptr;
    WidgetSkillSelection*   m_widgetSkillSelection = nullptr;
    std::vector<Character*> m_potentialCharacters;

    Texture* focusTexture = nullptr;
    Texture* noticeF      = nullptr;

    int  m_currentTargetIndex = 0;
    int  m_totalTargetIndex   = 0;
    bool m_bFirstTickElapsed  = false;
};
