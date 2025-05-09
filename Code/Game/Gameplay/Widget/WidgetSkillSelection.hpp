#pragma once
#include "Game/Framework/Widget.hpp"

class Texture;
class ISkill;
class WidgetSkillTargetSelection;
class StateSkillSelection;
class Character;

class WidgetSkillSelection : public Widget
{
public:
    WidgetSkillSelection(Character* character, StateSkillSelection* skillSelectionState); // We need character to determine the screen position of the Widget
    ~WidgetSkillSelection() override;

    void Draw() const override;
    void Update() override;

    bool SetIsSelectedSkill(bool isSelectedSkill);

    bool m_bIsSelectSkill   = false;
    bool m_bIsPushExecution = false;

protected:
    void UpdateKeyInput();

private:
    Character* m_character   = nullptr;
    Vec2       m_pivotLeftUp = Vec2::ZERO;
    float      m_opacity     = 1.0f;

    WidgetSkillTargetSelection* m_widgetTargetSelection = nullptr;

    /// Skill Data
    StateSkillSelection* m_skillSelectionState = nullptr;
    int                  m_currentNav          = 0;
    int                  m_totalNav            = 0;

    Texture*    texture    = nullptr;
    Texture*    selected   = nullptr;
    Texture*    noticeHand = nullptr;
    Texture*    noticeF    = nullptr;
    Texture*    noticeQ    = nullptr;
    BitmapFont* g_testFont = nullptr;

    /// Widget Geometry
    std::vector<AABB2> m_geometry;
};
