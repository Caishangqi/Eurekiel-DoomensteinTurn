#pragma once
#include "Game/Definition/SceneDefinition.hpp"
#include "Game/Framework/Widget.hpp"

class Character;

///
/// Use for per Character Buff and Health Display, typically this Widget
/// should inject Character and determine whether or not to display Health
/// and mana related information.
///
/// For Buff and Shield or Weakness, it should dynamically create new Icon hud
/// to show or remove. (in the world to screen)
class WidgetCharacterStateHud : public Widget
{
public:
    WidgetCharacterStateHud(Character* inCharacter);
    ~WidgetCharacterStateHud() override;

    void Update() override;
    void Draw() const override;
    void OnInit() override;

    bool GetCreateHealthIndicator();

    bool SetCreateHealthIndicator(bool newState);

protected:
    void CreateWidgetGeometry();

    void DrawHud() const;
    void DrawHudName(AABB2 hudBase) const;
    void DrawHudHealth(AABB2 hudBase) const;
    void DrawHudSP(AABB2 hudBase) const;

private:
    Character*    m_character               = nullptr;
    bool          m_isCreateHealthIndicator = false;
    CharacterSlot m_characterSlot;
    bool          m_bIsCharacterSelection = false;
    /// HeathBar
    AABB2 m_hudBound;
    Vec2  m_heathBarDimensions = Vec2(260, 120);
    float m_heathBarPaddingTop = 20.f;

    /// Font
    BitmapFont* g_testFont        = nullptr;
    Texture*    m_hudBoundTexture = nullptr;
};
