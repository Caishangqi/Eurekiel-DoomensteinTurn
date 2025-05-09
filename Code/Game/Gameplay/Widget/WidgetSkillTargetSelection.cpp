#include "WidgetSkillTargetSelection.hpp"

#include "WidgetSkillSelection.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/Skill/ISkill.hpp"
#include "Game/Gameplay/Battle/State/StateSkillExecution.hpp"


WidgetSkillTargetSelection::WidgetSkillTargetSelection(Character* inCharacter, WidgetSkillSelection* inSkillSelectionWidget): m_character(inCharacter), m_widgetSkillSelection(inSkillSelectionWidget)
{
    m_name                                   = "WidgetSkillTargetSelection";
    m_widgetSkillSelection->m_bIsSelectSkill = true;
    WidgetSkillTargetSelection::SetUserFocus();
    focusTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/Hand_2.png");
    noticeF      = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-f.png");
}

WidgetSkillTargetSelection::~WidgetSkillTargetSelection()
{
    m_widgetSkillSelection = nullptr;
    m_character            = nullptr;
    m_widgetSkillSelection = nullptr;
    m_potentialCharacters.clear();
}

void WidgetSkillTargetSelection::Draw() const
{
    Widget::Draw();
    if (m_potentialCharacters.size() == 0)
    {
        return;
    }
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);
    AABB2 targetPopBound;


    Vec3    slotPosition  = m_potentialCharacters[m_currentTargetIndex]->GetSlot().m_slotCenter;
    Camera* worldCam      = m_character->GetController()->m_worldCamera;
    Vec2    size          = m_potentialCharacters[m_currentTargetIndex]->GetCharacterDefinition()->m_size;
    Vec3    offSet        = slotPosition + Vec3(size.x * 1.6f, 0.f, size.y * 0.4f);
    Vec2    m_pivotLeftUp = worldCam->WorldToScreen(offSet, g_theGame->m_clientSpace.GetDimensions());
    targetPopBound.SetCenter(m_pivotLeftUp);
    targetPopBound.SetDimensions(Vec2(60.f, 60.f));
    AddVertsForAABB2D(vertices, targetPopBound, Rgba8::WHITE);
    g_theRenderer->BindTexture(focusTexture);
    g_theRenderer->DrawVertexArray(vertices);

    std::vector<Vertex_PCU> verticesNotice;
    verticesNotice.reserve(1024);
    AABB2 noticePopBound = targetPopBound;
    noticePopBound.SetDimensions(Vec2(50.f, 50.f));
    noticePopBound.SetCenter(targetPopBound.GetCenter() + Vec2(70.f, 0.f));
    AddVertsForAABB2D(verticesNotice, noticePopBound, Rgba8::WHITE);
    g_theRenderer->BindTexture(noticeF);
    g_theRenderer->DrawVertexArray(verticesNotice);


    /*
    AddVertsForAABB2D(vertices, g_theGame->m_clientSpace, Rgba8(0, 0, 0, 96));
    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(vertices);*/
}

void WidgetSkillTargetSelection::Update()
{
    Widget::Update();
    UpdateKeyInput();
    if (m_bFirstTickElapsed == false)
        m_bFirstTickElapsed = true;
}

void WidgetSkillTargetSelection::UpdateKeyInput()
{
    if (!m_bIsFocused) return;
    if (g_theInput->WasKeyJustPressed('Q'))
    {
        m_selectedSkill                          = nullptr;
        m_bIsFocused                             = false;
        m_widgetSkillSelection->m_bIsSelectSkill = false;
        m_widgetSkillSelection->SetUserFocus();
        RemoveFromViewport(); // We still need a handle like Actor Handle to fully manage Widget lifecycle.
    }
    if (!m_bIsFocused) return;
    if (g_theInput->WasKeyJustPressed(KEYCODE_UPARROW) || g_theInput->WasKeyJustPressed('W'))
    {
        m_currentTargetIndex--;
        if (m_currentTargetIndex < 0) m_currentTargetIndex = m_totalTargetIndex - 1;
    }
    if (g_theInput->WasKeyJustPressed(KEYCODE_DOWNARROW) || g_theInput->WasKeyJustPressed('S'))
    {
        m_currentTargetIndex++;
        if (m_currentTargetIndex >= m_totalTargetIndex) m_currentTargetIndex = 0;
    }

    if (g_theInput->WasKeyJustPressed('F') && m_bFirstTickElapsed)
    {
        /// Fill The target of selection
        m_selectedSkill->AddToTarget(m_potentialCharacters[m_currentTargetIndex]);
        g_theGame->m_turnBaseSubsystem->PushState(new StateSkillExecution(m_selectedSkill, m_character));
        m_widgetSkillSelection->m_bIsPushExecution = true;
        m_selectedSkill                            = nullptr;
        m_widgetSkillSelection->m_bIsSelectSkill   = false;
        m_widgetSkillSelection->SetUserFocus();
        RemoveFromViewport();
    }
}

void WidgetSkillTargetSelection::UpdateSelectSkill(ISkill* inSkill)
{
    if (m_selectedSkill)
    {
        delete m_selectedSkill;
        m_selectedSkill = nullptr;
    }
    m_selectedSkill       = inSkill;
    m_potentialCharacters = m_selectedSkill->GetPotentialTargets();
    m_totalTargetIndex    = static_cast<int>(m_potentialCharacters.size());
}

bool WidgetSkillTargetSelection::GetIsSelectSkill() const
{
    return m_selectedSkill != nullptr;
}
