﻿#include "Game/App.hpp"

#include "Engine/Window/Window.hpp"
#include "Game.hpp"
#include "Framework/PlayerController.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Framework/LoggerSubsystem.hpp"
#include "Framework/ResourceSubsystem.hpp"
#include "Framework/WidgetSubsystem.hpp"
#include "Gameplay/Save/PlayerSaveSubsystem.hpp"

Window*                g_theWindow              = nullptr;
Renderer*              g_theRenderer            = nullptr;
App*                   g_theApp                 = nullptr;
RandomNumberGenerator* g_rng                    = nullptr;
InputSystem*           g_theInput               = nullptr;
AudioSystem*           g_theAudio               = nullptr;
Game*                  g_theGame                = nullptr;
WidgetSubsystem*       g_theWidgetSubsystem     = nullptr;
ResourceSubsystem*     g_theResourceSubsystem   = nullptr;
PlayerSaveSubsystem*   g_thePlayerSaveSubsystem = nullptr;
LoggerSubsystem*       g_theLoggerSubsystem     = nullptr;

App::App()
{
}

App::~App()
{
    // Delete and release the render instance
    delete g_theRenderer;
    g_theRenderer = nullptr;


    delete g_theInput;
    g_theInput = nullptr;
}

void App::Startup()
{
    // Load Game Config
    LoadGameConfig("Data/GameConfig.xml");
    m_consoleSpace.m_mins = Vec2::ZERO;
    m_consoleSpace.m_maxs = Vec2(g_gameConfigBlackboard.GetValue("screenSizeX", 1600.f), g_gameConfigBlackboard.GetValue("screenSizeY", 800.f));

    LoggerConfig loggerConfig;
    g_theLoggerSubsystem = new LoggerSubsystem(loggerConfig);

    EventSystemConfig eventSystemConfig;
    g_theEventSystem = new EventSystem(eventSystemConfig);
    g_theEventSystem->SubscribeEventCallbackFunction("WindowCloseEvent", WindowCloseEvent); // Subscribe the WindowCloseEvent
    g_theEventSystem->SubscribeEventCallbackFunction("Event.Console.Startup", Event_ConsoleStartup);

    // Create All Engine Subsystems
    InputSystemConfig inputConfig;
    g_theInput = new InputSystem(inputConfig);

    WindowConfig windowConfig;
    windowConfig.m_aspectRatio = 2.f;
    windowConfig.m_inputSystem = g_theInput;
    windowConfig.m_windowTitle = "Doomenstein";
    g_theWindow                = new Window(windowConfig);

    RenderConfig renderConfig;
    renderConfig.m_window = g_theWindow;
    g_theRenderer         = new Renderer(renderConfig); // Create render

    DebugRenderConfig debugRenderConfig;
    debugRenderConfig.m_renderer = g_theRenderer;

    WidgetSystemConfig widgetSystemConfig;
    widgetSystemConfig.m_viewportCamera         = new Camera();
    widgetSystemConfig.m_viewportCamera->m_mode = eMode_Orthographic;
    widgetSystemConfig.m_viewportCamera->SetOrthographicView(Vec2::ZERO, Vec2(g_gameConfigBlackboard.GetValue("screenSizeX", 1600.f), g_gameConfigBlackboard.GetValue("screenSizeY", 800.f)));
    g_theWidgetSubsystem = new WidgetSubsystem(widgetSystemConfig);

    DevConsoleConfig consoleConfig;
    consoleConfig.renderer         = g_theRenderer;
    consoleConfig.m_camera         = new Camera();
    consoleConfig.m_camera->m_mode = eMode_Orthographic;
    consoleConfig.m_camera->SetOrthographicView(m_consoleSpace.m_mins, m_consoleSpace.m_maxs);
    g_theDevConsole = new DevConsole(consoleConfig);

    AudioSystemConfig audioConfig;
    g_theAudio = new AudioSystem(audioConfig);

    ResourceSystemConfig resourceConfig;
    g_theResourceSubsystem = new ResourceSubsystem(resourceConfig);

    g_thePlayerSaveSubsystem = new PlayerSaveSubsystem();

    g_theLoggerSubsystem->Startup();
    g_theEventSystem->Startup();
    g_theDevConsole->Startup();
    g_theInput->Startup();
    g_theWindow->Startup();
    g_theRenderer->Startup();
    g_theWidgetSubsystem->Startup();
    DebugRenderSystemStartup(debugRenderConfig);
    g_theAudio->Startup();
    g_theResourceSubsystem->Startup();

    g_theGame = new Game();
    g_rng     = new RandomNumberGenerator();
}

void App::Shutdown()
{
    /*
     *  All Destroy and ShutDown process should be reverse order of the StartUp
     */

    // Destroy the game
    delete g_theGame;
    g_theGame = nullptr;
    // Shutdown all Engine Subsystem
    g_theAudio->Shutdown();
    g_theDevConsole->Shutdown();
    DebugRenderSystemShutdown();
    g_theRenderer->Shutdown();
    g_theWidgetSubsystem->Shutdown();
    g_theWindow->Shutdown();
    g_theInput->Shutdown();
    g_theEventSystem->Shutdown();
    g_theResourceSubsystem->Shutdown();
    g_theLoggerSubsystem->Shutdown();
    // Destroy all Engine Subsystem
    delete g_theAudio;
    g_theAudio = nullptr;

    delete g_theDevConsole;
    g_theDevConsole = nullptr;

    delete g_theRenderer;
    g_theRenderer = nullptr;

    delete g_theWindow;
    g_theWindow = nullptr;

    delete g_theWidgetSubsystem;
    g_theWidgetSubsystem = nullptr;

    delete g_theResourceSubsystem;
    g_theResourceSubsystem = nullptr;

    delete g_theInput;
    g_theInput = nullptr;

    delete g_theEventSystem;
    g_theEventSystem = nullptr;

    delete g_theLoggerSubsystem;
    g_theLoggerSubsystem = nullptr;
}

void App::RunFrame()
{
    BeginFrame(); //Engine pre-frame stuff
    Update(); // Game updates / moves / spawns / hurts
    Render(); // Game draws current state of things
    EndFrame(); // Engine post-frame
}

bool App::IsQuitting() const
{
    return m_isQuitting;
}

void App::HandleQuitRequested()
{
    m_isQuitting = true;
}

void App::HandleKeyBoardEvent()
{
    if (g_theInput->WasKeyJustPressed(0x70))
    {
        m_isDebug = !m_isDebug;
    }
    /*if (g_theInput->WasKeyJustPressed(0x77))
    {
        m_isPendingRestart = true;
    }*/
    if (g_theInput->WasKeyJustPressed('O'))
    {
        m_isPaused = true;
        g_theGame->m_clock->StepSingleFrame();
    }
}

bool App::Event_ConsoleStartup(EventArgs& args)
{
    UNUSED(args)
    g_theDevConsole->AddLine(DevConsole::COLOR_INPUT_NORMAL, Stringf("Protogame3D Key Mappings:"));
    g_theDevConsole->AddLine(Rgba8(95, 95, 95),

                             "Mouse   - Aim\n"
                             "W/A     - Move\n"
                             "S/D     - Strafe\n"
                             "Q/E     - Roll\n"
                             "Z/C     - Elevate\n"
                             "Shift   - Sprint\n"
                             "LMB     - 10 Unit Raycast towards camera orientation\n"
                             "RMB     - 0.25 Unit Raycast towards camera orientation\n"
                             "F1      - Posses Test Actor \n"
                             "H       - Reset position and orientation to zero\n"
                             "N       - Start the Game (In Menu)\n"
                             "ESC     - Quit\n"
                             "P       - Pause the Game\n"
                             "O       - Step single frame\n"
                             "T       - Toggle time scale between 0.1 and 1.0\n"
                             "~       - Toggle Develop Console");
    return true;
}

void App::AdjustForPauseAndTimeDistortion()
{
    m_isSlowMo = g_theInput->IsKeyDown('T');
    if (m_isSlowMo)
    {
        g_theGame->m_clock->SetTimeScale(0.1f);
    }
    else
    {
        g_theGame->m_clock->SetTimeScale(1.0f);
    }


    if (g_theInput->WasKeyJustPressed('P'))
    {
        m_isPaused = !m_isPaused;
    }

    if (m_isPaused)
    {
        g_theGame->m_clock->Pause();
    }
    else
    {
        g_theGame->m_clock->Unpause();
    }
}


void App::BeginFrame()
{
    Clock::TickSystemClock();
    g_theInput->BeginFrame();
    g_theWindow->BeginFrame();
    g_theRenderer->BeginFrame();
    g_theWidgetSubsystem->BeginFrame();
    DebugRenderBeginFrame();
    g_theAudio->BeginFrame();
    g_theResourceSubsystem->BeginFrame();
    g_theEventSystem->BeginFrame();
    g_theDevConsole->BeginFrame();
}

void App::UpdateCameras()
{
}

void App::Update()
{
    /// Cursor
    auto windowHandle   = static_cast<HWND>(g_theWindow->GetWindowHandle());
    bool windowHasFocus = (GetActiveWindow() == windowHandle);

    bool devConsoleOpen = (g_theDevConsole->GetMode() != DevConsoleMode::HIDDEN);
    if (!windowHasFocus || devConsoleOpen)
    {
        g_theInput->SetCursorMode(CursorMode::POINTER);
    }
    else
    {
        g_theInput->SetCursorMode(CursorMode::FPS);
    }

    /// 

    HandleKeyBoardEvent();
    AdjustForPauseAndTimeDistortion();
    g_theGame->Update();
}

// If this methods is const, the methods inn the method should also promise
// const
void App::Render() const
{
    g_theRenderer->ClearScreen(Rgba8(m_backgroundColor));
    g_theGame->Render();
    g_theDevConsole->Render(m_consoleSpace);
}

void App::EndFrame()
{
    g_theWindow->EndFrame();
    g_theRenderer->EndFrame();
    g_theWidgetSubsystem->EndFrame();
    DebugRenderEndFrame();
    g_theInput->EndFrame();
    g_theAudio->EndFrame();
    g_theResourceSubsystem->EndFrame();
    g_theEventSystem->EndFrame();
    g_theDevConsole->EndFrame();
    g_theGame->EndFrame();

    if (m_isPendingRestart)
    {
        delete g_theGame;
        g_theGame = nullptr;
        g_theGame = new Game();
        // Restore state
        m_isPendingRestart = false;
        m_isPaused         = false;
    }
}

void App::LoadGameConfig(const char* filename)
{
    XmlDocument gameConfigXml;
    XmlResult   result = gameConfigXml.LoadFile(filename);
    if (result == XmlResult::XML_SUCCESS)
    {
        XmlElement* rootElement = gameConfigXml.RootElement();
        if (rootElement)
        {
            g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*rootElement);
            DebuggerPrintf("App::LoadGameConfig    Game from \"%s\" was loaded\n", filename);
        }
        else
        {
            DebuggerPrintf("App::LoadGameConfig    Game from \"%s\"was invalid (missing root element)\n", filename);
        }
    }
    else
    {
        DebuggerPrintf("App::LoadGameConfig    Failed to load game from \"%s\"\n", filename);
    }
}

bool App::WindowCloseEvent(EventArgs& args)
{
    UNUSED(args)
    g_theApp->HandleQuitRequested();
    return false;
}
