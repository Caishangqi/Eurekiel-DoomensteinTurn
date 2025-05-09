#include "WidgetDamageIndicator.hpp"

#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"


WidgetDamageIndicator::WidgetDamageIndicator()
{
    g_testFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
}

void WidgetDamageIndicator::Draw() const
{
    Widget::Draw();
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(256);
    AABB2 boundingBox;
    boundingBox.SetCenter(m_screenPos);
    boundingBox.SetDimensions(m_dimensions);
    g_testFont->AddVertsForTextInBox2D(vertices, m_data, boundingBox, m_size, m_interpolateColor, 0.7f);
    g_theRenderer->BindTexture(&g_testFont->GetTexture());
    g_theRenderer->DrawVertexArray(vertices);
}

void WidgetDamageIndicator::Update()
{
    Widget::Update();

    if (m_timer->HasPeriodElapsed())
    {
        RemoveFromViewport();
    }
    else
    {
        if (m_bInterpolate)
        {
            m_interpolateColor = Interpolate(m_startColor, m_endColor, m_timer->GetElapsedFraction());
        }
        m_screenPos = g_theGame->GetLocalPlayer(1)->m_worldCamera->WorldToScreen(m_position, g_theGame->m_clientSpace.GetDimensions());
    }
}

WidgetDamageIndicator::~WidgetDamageIndicator()
{
    m_timer->Stop();
    delete m_timer;
    m_timer = nullptr;
}

void WidgetDamageIndicator::OnInit()
{
    Widget::OnInit();
    m_timer = new Timer(m_duration);
    m_timer->Start();
}
