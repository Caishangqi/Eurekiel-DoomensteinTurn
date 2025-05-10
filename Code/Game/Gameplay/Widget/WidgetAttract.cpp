#include "WidgetAttract.hpp"

#include "WidgetLobby.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Framework/ResourceSubsystem.hpp"
#include "Game/Framework/Sound.hpp"
#include "Game/Framework/WidgetSubsystem.hpp"

WidgetAttract::WidgetAttract()
{
    m_name         = "WidgetAttract";
    m_textureTitle = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Title.png");
    g_theEventSystem->SubscribeEventCallbackFunction("GameStateChangeEvent", OnStateChange);
    m_lowerInfoStringVertex.reserve(1024);
    SoundID         mainMenuSoundID    = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("mainMenuMusic", ""));
    SoundPlaybackID mainMenuPlaybackID = g_theAudio->StartSound(mainMenuSoundID, true, 0.5f);
    g_theResourceSubsystem->CachedSoundPlaybackID(mainMenuPlaybackID, mainMenuSoundID);
    m_TitleBounds.SetDimensions(Vec2(960, 540));
    m_TitleBounds.SetCenter(Vec2(800, 400));
}

WidgetAttract::~WidgetAttract()
{
    g_theEventSystem->UnsubscribeEventCallbackFunction("GameStateChangeEvent", OnStateChange);
}

void WidgetAttract::Draw() const
{
    Widget::Draw();
    g_theRenderer->ClearScreen(g_theApp->m_backgroundColor);
    g_theRenderer->BindTexture(nullptr);
    DebugDrawRing(Vec2(800, 400), m_currentIconCircleThickness, m_currentIconCircleThickness / 10, Rgba8::WHITE);

    /// Add Lower Info String
    BitmapFont* g_testFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
    g_theRenderer->BindTexture(&g_testFont->GetTexture());
    g_theRenderer->DrawVertexArray(m_lowerInfoStringVertex);

    std::vector<Vertex_PCU> titleVertices;
    titleVertices.reserve(1024);
    AddVertsForAABB2D(titleVertices, m_TitleBounds, Rgba8::WHITE);
    float degrees   = FluctuateValue(5, 10, 2, g_theGame->m_clock->GetTotalSeconds());
    float scale     = FluctuateValue(1.0f, 0.25f, 1.5, g_theGame->m_clock->GetTotalSeconds());
    Mat44 transform = Mat44::MakeUniformScale2D(scale);
    transform.AppendZRotation(degrees);
    TransformVertexArray3D(titleVertices, transform);


    g_theRenderer->BindTexture(m_textureTitle);
    g_theRenderer->DrawVertexArray(titleVertices);
}

void WidgetAttract::Update()
{
    Widget::Update();

    /// Graphic Lower Info String Drawing
    BitmapFont* g_testFont  = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
    AABB2       boundingBox = g_theGame->m_clientSpace;
    m_lowerInfoStringVertex.clear();
    g_testFont->AddVertsForTextInBox2D(m_lowerInfoStringVertex, m_lowerInfoString, boundingBox, 15.f, Rgba8::WHITE, 1, Vec2(0.5f, 0.f));

    m_counter++;
    m_currentIconCircleThickness = FluctuateValue(m_iconCircleRadius, 50.f, 0.02f, static_cast<float>(m_counter));

    UpdateKeyInput();
}

bool WidgetAttract::OnStateChange(EventArgs& args)
{
    UNUSED(args)
    if (g_theGame->m_currentState != GameState::ATTRACT)
    {
        g_theWidgetSubsystem->RemoveFromViewport("WidgetAttract");
        return true;
    }
    return false;
}

void WidgetAttract::UpdateKeyInput()
{
    const XboxController& controller = g_theInput->GetController(0);

    bool spaceBarPressed     = g_theInput->WasKeyJustPressed(32);
    bool startButtonPressed  = controller.WasButtonJustPressed(XBOX_BUTTON_START);
    bool escapeButtonPressed = g_theInput->WasKeyJustPressed(KEYCODE_ESC);
    bool backButtonPressed   = controller.WasButtonJustPressed(XBOX_BUTTON_BACK);

    if (escapeButtonPressed || backButtonPressed)
    {
        g_theEventSystem->FireEvent("WindowCloseEvent");
    }

    if (spaceBarPressed || startButtonPressed)
    {
        auto lobbyWidget = new WidgetLobby();

        if (spaceBarPressed)
        {
            g_theGame->CreateLocalPlayer(1, DeviceType::KEYBOARD_AND_MOUSE);
            g_theWidgetSubsystem->AddToViewport(lobbyWidget);
            RemoveFromViewport();
        }
        else if (startButtonPressed)
        {
            g_theGame->CreateLocalPlayer(1, DeviceType::CONTROLLER);
            g_theWidgetSubsystem->AddToViewport(lobbyWidget);
            RemoveFromViewport();
        }
        PLAY_SOUND_CLICK("buttonClickSound");
    }
}

float WidgetAttract::FluctuateValue(float value, float amplitude, float frequency, float deltaTime) const
{
    return value + amplitude * sinf(frequency * deltaTime);
}
