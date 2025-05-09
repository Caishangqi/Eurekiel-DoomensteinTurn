#include "WidgetSkillSelection.hpp"

#include "WidgetSkillTargetSelection.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/WidgetSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/Skill/ISkill.hpp"
#include "Game/Gameplay/Battle/State/StateSkillSelection.hpp"


WidgetSkillSelection::WidgetSkillSelection(Character* character, StateSkillSelection* skillSelectionState): m_character(character), m_skillSelectionState(skillSelectionState)
{
    m_name       = "WidgetSkillSelection";
    m_totalNav   = static_cast<int>(character->GetSkills().size());
    m_currentNav = 0;
    m_geometry.reserve(m_totalNav);
    texture    = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/skill-lable.png");
    selected   = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/skill-lable-selected.png");
    noticeHand = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/Hand_0.png");
    noticeF    = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-f.png");
    noticeQ    = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-q.png");
    g_testFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
}

WidgetSkillSelection::~WidgetSkillSelection()
{
}

void WidgetSkillSelection::Draw() const
{
    Widget::Draw();
    for (int i = 0; i < static_cast<int>(m_geometry.size()); i++)
    {
        if (m_currentNav == i)
        {
            std::vector<Vertex_PCU> selectedVertices;
            selectedVertices.reserve(1024);
            AABB2 selectedBox = m_geometry[i];
            selectedBox.SetDimensions(Vec2(170.f, 55.f));
            Rgba8 selectColor = Rgba8::WHITE;
            if (m_bIsSelectSkill)
                selectColor = Rgba8::ORANGE;
            AddVertsForAABB2D(selectedVertices, selectedBox, selectColor);
            g_theRenderer->BindTexture(selected);
            g_theRenderer->DrawVertexArray(selectedVertices);

            std::vector<Vertex_PCU> noticeFVertices;
            noticeFVertices.reserve(1024);
            AABB2    noticeFBox = selectedBox;
            Texture* noticeIcon = noticeF;
            if (m_bIsSelectSkill)
                noticeIcon = noticeQ;
            noticeFBox.SetDimensions(Vec2(40.f, 40.f));
            noticeFBox.SetCenter(selectedBox.GetCenter() + Vec2(100.f, 0));
            AddVertsForAABB2D(noticeFVertices, noticeFBox, Rgba8::WHITE);
            g_theRenderer->BindTexture(noticeIcon);
            g_theRenderer->DrawVertexArray(noticeFVertices);

            if (!m_bIsSelectSkill)
            {
                std::vector<Vertex_PCU> noticeVertices;
                noticeVertices.reserve(1024);
                AABB2 noticeBox = selectedBox;
                noticeBox.SetDimensions(Vec2(40.f, 40.f));
                noticeBox.SetCenter(selectedBox.GetCenter() - Vec2(90.f, 0));
                AddVertsForAABB2D(noticeVertices, noticeBox, Rgba8::WHITE);
                g_theRenderer->BindTexture(noticeHand);
                g_theRenderer->DrawVertexArray(noticeVertices);
            }
        }
        else
        {
            std::vector<Vertex_PCU> deSelectedVertices;
            deSelectedVertices.reserve(1024);
            AABB2 tempGeometry = m_geometry[i];
            tempGeometry.SetDimensions(Vec2(160.f, 32.f));
            AddVertsForAABB2D(deSelectedVertices, m_geometry[i], Rgba8::WHITE);
            g_theRenderer->BindTexture(texture);
            g_theRenderer->DrawVertexArray(deSelectedVertices);
        }
    }


    std::vector<Vertex_PCU> fontVertices;
    fontVertices.reserve(1024);
    for (int i = 0; i < static_cast<int>(m_geometry.size()); i++)
    {
        std::string skillAlias = m_character->GetSkills()[i]->m_alias;
        g_testFont->AddVertsForTextInBox2D(fontVertices, Stringf("%s", skillAlias.c_str()), m_geometry[i], 14.f, Rgba8::WHITE, 0.6f, Vec2(0.1f, 0.5f));
    }

    g_theRenderer->BindTexture(&g_testFont->GetTexture());
    g_theRenderer->BindShader(nullptr);
    g_theRenderer->DrawVertexArray(fontVertices);
}

void WidgetSkillSelection::Update()
{
    Widget::Update();
    if (m_bIsPushExecution)
    {
        m_skillSelectionState->SetIsFinished(true);
        RemoveFromViewport();
        return;
    }
    Vec3    characterPosOff = m_character->GetPosition() + Vec3(0.f, -0.5f, 0.5f);
    Camera* worldCam        = m_character->GetController()->m_worldCamera;
    m_pivotLeftUp           = worldCam->WorldToScreen(characterPosOff, g_theGame->m_clientSpace.GetDimensions());

    m_geometry.clear();
    m_geometry.reserve(m_totalNav);
    for (int i = 0; i < m_totalNav; ++i)
    {
        AABB2 skillBound;
        skillBound.m_mins = m_pivotLeftUp + static_cast<float>(i) * Vec2(0.f, -36.f);
        skillBound.m_maxs = skillBound.m_mins + Vec2(160.f, 32.f);
        m_geometry.push_back(skillBound);
    }
    UpdateKeyInput();
}

bool WidgetSkillSelection::SetIsSelectedSkill(bool isSelectedSkill)
{
    m_bIsSelectSkill = isSelectedSkill;
    return m_bIsSelectSkill;
}

void WidgetSkillSelection::UpdateKeyInput()
{
    if (!m_bIsFocused) return;
    if (g_theInput->WasKeyJustPressed(KEYCODE_UPARROW) || g_theInput->WasKeyJustPressed('W'))
    {
        m_currentNav--;
        if (m_currentNav < 0) m_currentNav = m_totalNav - 1;
    }
    if (g_theInput->WasKeyJustPressed(KEYCODE_DOWNARROW) || g_theInput->WasKeyJustPressed('S'))
    {
        m_currentNav++;
        if (m_currentNav >= m_totalNav) m_currentNav = 0;
    }

    if (g_theInput->WasKeyJustPressed('F'))
    {
        if (m_bIsSelectSkill)
        {
            return;
        }
        m_widgetTargetSelection = new WidgetSkillTargetSelection(m_character, this);
        m_widgetTargetSelection->UpdateSelectSkill(m_character->GetSkills()[m_currentNav]->Duplicate());
        g_theWidgetSubsystem->AddToViewport(m_widgetTargetSelection); // Consider to automatic this process by using widget subsystem.
        m_bIsFocused = false;
    }
}
