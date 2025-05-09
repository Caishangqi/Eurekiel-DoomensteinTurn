#include "WidgetCharacterStateHud.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/State/StateSkillSelection.hpp"

WidgetCharacterStateHud::WidgetCharacterStateHud(Character* inCharacter): m_character(inCharacter)
{
    m_characterSlot   = m_character->GetSlot();
    g_testFont        = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
    m_hudBoundTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/character-hud.png");
}

WidgetCharacterStateHud::~WidgetCharacterStateHud()
{
}

void WidgetCharacterStateHud::Update()
{
    Widget::Update();
    auto state = dynamic_cast<StateSkillSelection*>(g_theGame->m_turnBaseSubsystem->GetStateStackBack());
    if (state && state->GetCharacter() == m_character)
    {
        m_bIsCharacterSelection = true;
    }
    else
    {
        m_bIsCharacterSelection = false;
    }
}

void WidgetCharacterStateHud::Draw() const
{
    Widget::Draw();
    if (m_isCreateHealthIndicator)
    {
        DrawHud();
    }
}

void WidgetCharacterStateHud::OnInit()
{
    Widget::OnInit();
    CreateWidgetGeometry();
}

bool WidgetCharacterStateHud::GetCreateHealthIndicator()
{
    return m_isCreateHealthIndicator;
}

bool WidgetCharacterStateHud::SetCreateHealthIndicator(bool newState)
{
    m_isCreateHealthIndicator = newState;
    return newState;
}

void WidgetCharacterStateHud::CreateWidgetGeometry()
{
    if (m_isCreateHealthIndicator)
    {
        if (m_character->GetCharacterDefinition()->m_faction == "Friend") // Alignment to Right
        {
            AABB2 clientSpace = g_theGame->m_clientSpace;
            m_hudBound.SetDimensions(m_heathBarDimensions);
            int   index = m_characterSlot.m_slotIndex;
            float x     = clientSpace.m_maxs.x - m_hudBound.GetDimensions().x * 0.5f - m_heathBarPaddingTop;
            float y     = clientSpace.m_maxs.y - static_cast<float>(index + 1) * m_heathBarDimensions.y + m_heathBarPaddingTop * 2;
            m_hudBound.SetCenter(Vec2(x, y));
            m_hudBound = m_hudBound.GetPadded(Vec4(0.f, -5.f, 0.f, -5.f));
        }
    }
}

void WidgetCharacterStateHud::DrawHud() const
{
    AABB2 tempHudBound = m_hudBound;

    if (m_bIsCharacterSelection)
    {
        tempHudBound.SetCenter(m_hudBound.GetCenter() - Vec2(50.f, 0.f));
    }

    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);
    if (m_character->GetIsDead())
    {
        AddVertsForAABB2D(vertices, tempHudBound, Rgba8(255, 0, 0, 160));
    }
    else
    {
        AddVertsForAABB2D(vertices, tempHudBound, Rgba8(255, 255, 255, 160));
    }
    g_theRenderer->SetBlendMode(BlendMode::ALPHA);
    g_theRenderer->BindTexture(m_hudBoundTexture);
    g_theRenderer->DrawVertexArray(vertices);

    DrawHudName(tempHudBound);
    DrawHudHealth(tempHudBound);
    DrawHudSP(tempHudBound);
}

void WidgetCharacterStateHud::DrawHudName(AABB2 hudBase) const
{
    AABB2 tempHudNameBound = hudBase;
    tempHudNameBound       = tempHudNameBound.GetPadded(Vec4(0.f, -50.f, 0.f, 0.f));
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);
    std::vector<Vertex_PCU> verticesText;
    verticesText.reserve(4096);

    //AddVertsForAABB2D(vertices, tempHudNameBound, Rgba8(255, 0, 0, 20));
    g_testFont->AddVertsForTextInBox2D(verticesText, m_character->GetCharacterDefinition()->m_name.c_str(), tempHudNameBound, 20.f, Rgba8::WHITE, 0.7f, Vec2(0.1f, 0.5f));
    g_theRenderer->SetBlendMode(BlendMode::ALPHA);
    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(vertices);
    g_theRenderer->BindTexture(&g_testFont->GetTexture());
    g_theRenderer->DrawVertexArray(verticesText);
}

void WidgetCharacterStateHud::DrawHudHealth(AABB2 hudBase) const
{
    /// Heath Bound
    AABB2 tempHudHeathBound = hudBase;
    tempHudHeathBound       = tempHudHeathBound.GetPadded(Vec4(-60.f, -45.f, -20.f, -35.f));
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);
    std::vector<Vertex_PCU> verticesText;
    verticesText.reserve(4096);
    //AddVertsForAABB2D(vertices, tempHudHeathBound, Rgba8(0, 255, 0, 20));

    /// Heath Text
    AABB2 tempHudHeathTextBound = tempHudHeathBound;
    tempHudHeathTextBound       = tempHudHeathTextBound.GetPadded(Vec4(0.f, -10.f, 0.f, 0.f));
    g_testFont->AddVertsForTextInBox2D(verticesText, "HP", tempHudHeathTextBound, 10.f, Rgba8::WHITE, 0.7f, Vec2(0.05f, 0.0f));
    g_testFont->AddVertsForTextInBox2D(verticesText, Stringf("%d/%d", m_character->GetCurrentHealth(), static_cast<int>(m_character->GetCharacterDefinition()->m_health)), tempHudHeathTextBound, 20.f,
                                       Rgba8::WHITE,
                                       1.0f, Vec2(1.f, 0.0f));

    /// Heath Bar
    AABB2 tempHudHeathBarBound = tempHudHeathBound;
    tempHudHeathBarBound       = tempHudHeathBarBound.GetPadded(Vec4(0.f, -5.f, 0.f, -20.f));
    std::vector<Vertex_PCU> tempHudHeathBarVertices;
    tempHudHeathBarVertices.reserve(1024);
    AddVertsForAABB2D(tempHudHeathBarVertices, tempHudHeathBarBound, Rgba8(88, 132, 92, 255));
    // Bar Gray
    AABB2 tempHudHeathBarGreyBound = tempHudHeathBound;
    float healthFraction           = static_cast<float>(m_character->GetCurrentHealth()) / m_character->GetCharacterDefinition()->m_health;
    float paddingX                 = tempHudHeathBound.GetDimensions().x * healthFraction;
    tempHudHeathBarGreyBound       = tempHudHeathBarGreyBound.GetPadded(Vec4(-paddingX, -5.f, 0.f, -20.f));
    AddVertsForAABB2D(tempHudHeathBarVertices, tempHudHeathBarGreyBound, Rgba8(44, 44, 44, 255));

    g_theRenderer->SetBlendMode(BlendMode::ALPHA);
    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(vertices);


    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(tempHudHeathBarVertices);

    g_theRenderer->BindTexture(&g_testFont->GetTexture());
    g_theRenderer->DrawVertexArray(verticesText);
}

void WidgetCharacterStateHud::DrawHudSP(AABB2 hudBase) const
{
    /// Heath Bound
    AABB2 tempHudSPBound = hudBase;
    tempHudSPBound       = tempHudSPBound.GetPadded(Vec4(-100.f, -25.f, -20.f, -60.f));
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);
    std::vector<Vertex_PCU> verticesText;
    verticesText.reserve(4096);
    //AddVertsForAABB2D(vertices, tempHudSPBound, Rgba8(0, 0, 255, 20));

    /// SP Text
    AABB2 tempHudSPTextBound = tempHudSPBound;
    tempHudSPTextBound       = tempHudSPTextBound.GetPadded(Vec4(0.f, -10.f, 0.f, 0.f));
    g_testFont->AddVertsForTextInBox2D(verticesText, "SP", tempHudSPTextBound, 10.f, Rgba8::WHITE, 0.7f, Vec2(0.05f, 0.0f));
    g_testFont->AddVertsForTextInBox2D(verticesText, Stringf("%d/%d", m_character->GetCurrentMana(), m_character->GetMaxMana()), tempHudSPTextBound, 20.f, Rgba8::WHITE,
                                       1.0f, Vec2(1.f, 0.0f));

    /// SP Bar
    AABB2 tempHudSPBarBound = tempHudSPBound;
    tempHudSPBarBound       = tempHudSPBarBound.GetPadded(Vec4(0.f, -5.f, 0.f, -15.f));
    std::vector<Vertex_PCU> tempHudSPBarVertices;
    tempHudSPBarVertices.reserve(1024);
    AddVertsForAABB2D(tempHudSPBarVertices, tempHudSPBarBound, Rgba8(80, 113, 127, 255));
    // Bar Gray
    AABB2 tempHudSPBarGreyBound = tempHudSPBound;
    float spFraction            = static_cast<float>(m_character->GetCurrentMana()) / static_cast<float>(m_character->GetMaxMana());
    float paddingX              = tempHudSPBound.GetDimensions().x * spFraction;
    tempHudSPBarGreyBound       = tempHudSPBarGreyBound.GetPadded(Vec4(-paddingX, -5.f, 0.f, -20.f));
    AddVertsForAABB2D(tempHudSPBarVertices, tempHudSPBarGreyBound, Rgba8(44, 44, 44, 255));

    g_theRenderer->SetBlendMode(BlendMode::ALPHA);
    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(vertices);


    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(tempHudSPBarVertices);

    g_theRenderer->BindTexture(&g_testFont->GetTexture());
    g_theRenderer->DrawVertexArray(verticesText);
}
