#include "WidgetWorldPopup.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/Definition/ActorDefinition.hpp"
#include "Game/Gameplay/Actor.hpp"

WidgetWorldPopup::WidgetWorldPopup(Vec3& position): m_worldPosition(position)
{
    m_name                  = "WidgetWorldPopup";
    m_textureInteractIcon   = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/Hand_2.png");
    m_textureInteractButton = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-f.png");
}

WidgetWorldPopup::WidgetWorldPopup(Vec3& position, Actor* pointActor): m_worldPosition(position), m_pointActor(pointActor)
{
    m_name                  = "WidgetWorldPopup";
    m_textureInteractIcon   = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/Hand_2.png");
    m_textureInteractButton = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-f.png");
}

WidgetWorldPopup::~WidgetWorldPopup()
{
    m_textureInteractButton = nullptr;
    m_textureInteractIcon   = nullptr;
}

void WidgetWorldPopup::Draw() const
{
    Widget::Draw();
    if (!g_theGame->m_map) return;
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);
    AABB2 targetPopBound;

    Camera* worldCam      = m_owner->m_worldCamera;
    Vec2    size          = m_pointActor->m_definition->m_size;
    Vec3    offSet        = m_pointActor->m_position + Vec3(0.f, -size.x * 0.6f, size.y * 0.6f);
    Vec2    m_pivotLeftUp = worldCam->WorldToScreen(offSet, g_theGame->m_clientSpace.GetDimensions());
    targetPopBound.SetCenter(m_pivotLeftUp);
    targetPopBound.SetDimensions(Vec2(60.f, 60.f));
    AddVertsForAABB2D(vertices, targetPopBound, Rgba8::WHITE);
    g_theRenderer->BindTexture(m_textureInteractIcon);
    g_theRenderer->DrawVertexArray(vertices);

    std::vector<Vertex_PCU> verticesNotice;
    verticesNotice.reserve(1024);
    AABB2 noticePopBound = targetPopBound;
    noticePopBound.SetDimensions(Vec2(50.f, 50.f));
    Vec2 newCenter = targetPopBound.GetCenter() + Vec2(70.f, 0.f);
    noticePopBound.SetCenter(newCenter);
    AddVertsForAABB2D(verticesNotice, noticePopBound, Rgba8::WHITE);
    g_theRenderer->BindTexture(m_textureInteractButton);
    g_theRenderer->DrawVertexArray(verticesNotice);
}

void WidgetWorldPopup::Update()
{
    Widget::Update();
}
