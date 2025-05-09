﻿#include <Game/Game.hpp>

#include "App.hpp"
#include "GameCommon.hpp"
#include "Framework/LoggerSubsystem.hpp"
#include "Framework/PlayerController.hpp"
#include "Prop.hpp"
#include "Definition/ActorDefinition.hpp"
#include "Definition/AnimationDefinition.hpp"
#include "Definition/CharacterDefinition.hpp"
#include "Definition/MapDefinition.hpp"
#include "Definition/SceneDefinition.hpp"
#include "Definition/TileDefinition.hpp"
#include "Definition/WeaponDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Framework/ResourceSubsystem.hpp"
#include "Framework/TurnBaseSubsystem.hpp"
#include "Framework/WidgetSubsystem.hpp"
#include "Gameplay/Map.hpp"
#include "Gameplay/Save/PlayerSaveSubsystem.hpp"
#include "Gameplay/Widget/WidgetAttract.hpp"
#include "Registration/SkillRegistration.hpp"

Game::Game()
{
    MapDefinition::LoadDefinitions("Data/Definitions/MapDefinitions.xml");
    TileDefinition::LoadDefinitions("Data/Definitions/TileDefinitions.xml");
    ActorDefinition::LoadDefinitions("Data/Definitions/ActorDefinitions.xml");
    ActorDefinition::LoadDefinitions("Data/Definitions/ProjectileActorDefinitions.xml");
    WeaponDefinition::LoadDefinitions("Data/Definitions/WeaponDefinitions.xml");
    SceneDefinition::LoadDefinitions("Data/Definitions/SceneDefinitions.xml");
    SkillRegistration::LoadRegistrations();
    AnimationDefinition::LoadDefinitions("Data/Definitions/AnimationDefinitions.xml");
    CharacterDefinition::LoadDefinitions("Data/Definitions/CharacterDefinitions.xml");
    /// Event Register
    g_theEventSystem->SubscribeEventCallbackFunction("GameExitEvent", GameExitEvent);
    g_theEventSystem->SubscribeEventCallbackFunction("GameStateChangeEvent", GameStateChangeEvent);
    /// 

    /// Resource
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Terrain_8x8.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Caizii.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/skill-lable.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/skill-lable-selected.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/skill-notice-up.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/skill-notice-down.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-f.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/key-q.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/Hand_0.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/Hand_2.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/character-hud.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/sequence-label-main.png");
    g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/sequence-label-sub.png");

    /// Rasterize
    g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);

    /// Spaces
    m_clientSpace.m_mins = Vec2::ZERO;
    m_clientSpace.m_maxs = Vec2(g_gameConfigBlackboard.GetValue("clientSizeX", 1600.f), g_gameConfigBlackboard.GetValue("clientSizeY", 800.f));

    /// Cameras
    m_debugScreenCamera = new Camera();

    m_debugScreenCamera->m_mode = eMode_Orthographic;
    m_debugScreenCamera->SetOrthographicView(Vec2::ZERO, m_clientSpace.m_maxs);

    Mat44 ndcMatrix;
    ndcMatrix.SetIJK3D(Vec3(0, 0, 1), Vec3(-1, 0, 0), Vec3(0, 1, 0));

    ///

    /// Clock
    m_clock = new Clock(Clock::GetSystemClock());
    ///

    /// Turn Base Subsystem
    TurnBaseSystemConfig turnBaseSystemConfig;
    m_turnBaseSubsystem = new TurnBaseSubsystem(turnBaseSystemConfig);
    m_turnBaseSubsystem->Startup();

    /// Game State
    g_theInput->SetCursorMode(CursorMode::POINTER);
    EnterState(GameState::ATTRACT);
}

Game::~Game()
{
    POINTER_SAFE_DELETE(m_map)
    POINTER_SAFE_DELETE(m_debugScreenCamera)
    MapDefinition::ClearDefinitions();
    ActorDefinition::ClearDefinitions();
    WeaponDefinition::ClearDefinitions();
    TileDefinition::ClearDefinitions();

    for (PlayerController* controller : m_localPlayerControllers)
    {
        POINTER_SAFE_DELETE(controller)
    }
}


void Game::Render() const
{
    g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
    g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
    g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);

    if (m_currentState == GameState::PLAYING)
    {
        for (PlayerController* player : m_localPlayerControllers)
        {
            player->Render();
            g_theRenderer->BeingCamera(*player->m_worldCamera);

            if (!m_turnBaseSubsystem->GetIsInBattle())
            {
                m_map->SetMapRenderContext(player);
                m_map->Render();
            }
            m_turnBaseSubsystem->Render();
            g_theRenderer->EndCamera(*player->m_worldCamera);
        }
        g_theRenderer->BindShader(nullptr);
        if (GetIsSingleMode())
            DebugRenderWorld(*g_theGame->m_localPlayerControllers[0]->m_worldCamera);
        DebugRenderScreen(*g_theGame->m_debugScreenCamera);
    }
    g_theWidgetSubsystem->Render();
}


void Game::UpdateCameras(float deltaTime)
{
    m_debugScreenCamera->Update(deltaTime);
}

void Game::UpdateListeners(float deltaTime)
{
    UNUSED(deltaTime)
    for (PlayerController* controller : m_localPlayerControllers)
    {
        Vec3 forward, left, up;
        controller->m_orientation.GetAsVectors_IFwd_JLeft_KUp(forward, left, up);
        g_theAudio->UpdateListener(controller->m_index - 1, controller->m_position, forward, up); // Index is an adjustment
    }
}


void Game::Update()
{
    if (m_currentState == GameState::ATTRACT)
    {
        g_theInput->SetCursorMode(CursorMode::POINTER);
    }

    /// PlayerController
    if (m_currentState == GameState::PLAYING)
    {
        for (PlayerController* controller : m_localPlayerControllers)
        {
            controller->Update(Clock::GetSystemClock().GetDeltaSeconds());
            DebugAddMessage(Stringf("PlayerController position: %.2f, %.2f, %.2f", controller->m_position.x, controller->m_position.y, controller->m_position.z), 0);
            DebugAddMessage(Stringf("PlayerController orientation: %.2f, %.2f, %.2f", controller->m_orientation.m_yawDegrees, controller->m_orientation.m_pitchDegrees,
                                    controller->m_orientation.m_rollDegrees),
                            0);
        }
        UpdateListeners(Clock::GetSystemClock().GetDeltaSeconds());
        m_turnBaseSubsystem->Update(); // Update TurnBase subsystem
    }
    ///

    /// Map
    if (!m_turnBaseSubsystem->GetIsInBattle() && m_map)
    {
        m_map->Update();
    }
    /// 

    /// Debug Only
    std::string debugGameState = Stringf("Time: %.2f FPS: %.1f Scale: %.2f",
                                         m_clock->GetTotalSeconds(),
                                         m_clock->GetFrameRate(),
                                         m_clock->GetTimeScale()
    );
    g_theWidgetSubsystem->Update();
    DebugAddScreenText(debugGameState, m_clientSpace, 14, 0);
    float deltaTime = m_clock->GetDeltaSeconds();
    UpdateCameras(deltaTime);
    HandleMouseEvent(deltaTime);
    HandleKeyBoardEvent(deltaTime);
}

void Game::EndFrame()
{
    if (m_map)
        m_map->EndFrame();
    if (m_turnBaseSubsystem)
        m_turnBaseSubsystem->EndFrame();
}


bool Game::GameExitEvent(EventArgs& args)
{
    UNUSED(args)
    printf("Event::GameStartEvent    Exiting game...\n");
    Game* game = g_theGame;
    delete game->m_map;
    game->m_map = nullptr;
    game->m_localPlayerControllers.clear();
    game->EnterState(GameState::ATTRACT);
    g_thePlayerSaveSubsystem->ClearSaves();
    g_theInput->SetCursorMode(CursorMode::POINTER);
    return true;
}

bool Game::GameStateChangeEvent(EventArgs& args)
{
    UNUSED(args)
    return false;
}

PlayerController* Game::CreateLocalPlayer(int id, DeviceType deviceType)
{
    auto newPlayer = new PlayerController(nullptr);
    newPlayer->SetInputDeviceType(deviceType);
    for (PlayerController* m_local_player_controller : m_localPlayerControllers)
    {
        if (m_local_player_controller && m_local_player_controller->GetControllerIndex() == id)
        {
            printf("Game::CreateLocalPlayer      You create the Player controller with same ID: %d\n", id);
            return nullptr;
        }
    }
    newPlayer->SetControllerIndex(id);
    m_localPlayerControllers.push_back(newPlayer);
    printf("Game::CreateLocalPlayer     Create Local Player with id: %d\n", id);
    return newPlayer;
}

void Game::RemoveLocalPlayer(int id)
{
    auto it = std::remove_if(m_localPlayerControllers.begin(), m_localPlayerControllers.end(),
                             [id](PlayerController* controller)
                             {
                                 if (controller && controller->GetControllerIndex() == id)
                                 {
                                     printf("Game::RemoveLocalPlayer     Remove Local Player with id: %d\n", id);
                                     delete controller;
                                     return true;
                                 }
                                 return false;
                             });
    // Another solution rather tha use remove_if
    // If you need to ensure that unused memory is freed, you can use the swap technique.
    m_localPlayerControllers.erase(it, m_localPlayerControllers.end());
    m_localPlayerControllers.shrink_to_fit();
}

PlayerController* Game::GetLocalPlayer(int id)
{
    for (PlayerController* m_local_player_controller : m_localPlayerControllers)
    {
        if (m_local_player_controller && m_local_player_controller->GetControllerIndex() == id)
            return m_local_player_controller;
    }
    return nullptr;
}

PlayerController* Game::GetControllerByDeviceType(DeviceType deviceType)
{
    for (PlayerController* m_local_player_controller : m_localPlayerControllers)
    {
        if (m_local_player_controller && m_local_player_controller->GetInputDeviceType() == deviceType)
            return m_local_player_controller;
    }
    return nullptr;
}

bool Game::GetIsSingleMode() const
{
    return m_localPlayerControllers.size() == 1;
}


void Game::EnterState(GameState state)
{
    EventArgs args;
    args.SetValue("State", std::to_string(static_cast<int>(state)));
    m_currentState = state;
    g_theEventSystem->FireEvent("GameStateChangeEvent", args);
    switch (state)
    {
    case GameState::PLAYING:
        EnterPlayingState();
        return;
    case GameState::ATTRACT:
        EnterAttractState();
        return;
    case GameState::LOBBY:
        EnterLobbyState();
        return;
    case GameState::NONE:
        printf("Game::EnterState    Enter game state: NONE\n");
        break;
    case GameState::COUNT:
        printf("Game::EnterState    Enter game state: COUNT\n");
        break;
    }
}

void Game::ExitState(GameState state)
{
    UNUSED(state)
}

void Game::EnterAttractState()
{
    SoundID mainMenuSoundID = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic", ""));
    g_theResourceSubsystem->ForceStopSoundAndRemoveSoundPlaybackID(mainMenuSoundID);
    Widget* attractWidget = new WidgetAttract();
    g_theWidgetSubsystem->AddToViewport(attractWidget);
}

void Game::EnterLobbyState()
{
}

void Game::EnterPlayingState()
{
    printf("Event::GameStartEvent    Starting game...\n");
    g_theInput->SetCursorMode(CursorMode::FPS);
    g_theAudio->SetNumListeners(static_cast<int>(m_localPlayerControllers.size()));
    std::string defaultMapName = g_gameConfigBlackboard.GetValue("defaultMap", "Default");
    m_map                      = new Map(this, MapDefinition::GetByName(defaultMapName));
    for (PlayerController* playerController : m_localPlayerControllers)
    {
        playerController->m_map = m_map;
    }
    SoundID         mainMenuSoundID    = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic", ""));
    SoundPlaybackID mainMenuPlaybackID = g_theAudio->StartSound(mainMenuSoundID, true, 0.25f);
    g_theResourceSubsystem->CachedSoundPlaybackID(mainMenuPlaybackID, mainMenuSoundID);
    for (PlayerController* player : m_localPlayerControllers)
    {
        PlayerSaveData saveData{};
        saveData.m_playerID = player->m_index;
        g_thePlayerSaveSubsystem->CreatePlayerSaveData(saveData);
    }
}


void Game::HandleKeyBoardEvent(float deltaTime)
{
    UNUSED(deltaTime)
    const XboxController& controller = g_theInput->GetController(0);

    if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || controller.WasButtonJustPressed(XBOX_BUTTON_BACK))
    {
        if (m_currentState == GameState::PLAYING)
        {
            PLAY_SOUND_CLICK("buttonClickSound");
            g_theEventSystem->FireEvent("GameExitEvent");
        }
    }
}

void Game::HandleMouseEvent(float deltaTime)
{
    UNUSED(deltaTime)
}
