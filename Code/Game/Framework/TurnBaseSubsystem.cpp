#include "TurnBaseSubsystem.hpp"

#include <cstdio>

#include "PlayerController.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/DebugRenderSystem.h"
#include "Game/Game.hpp"
#include "Game/Gameplay/Battle/Scene.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/SceneDefinition.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/CameraDirector.hpp"
#include "Game/Gameplay/Battle/State/ITurnState.hpp"
#include "Game/Gameplay/Battle/State/StateRound.hpp"


TurnBaseSubsystem::TurnBaseSubsystem(TurnBaseSystemConfig config): m_config(config)
{
    m_cameraDirector = new CameraDirector();
}

TurnBaseSubsystem::~TurnBaseSubsystem()
{
}

void TurnBaseSubsystem::EndFrame()
{
    if (m_scene) m_scene->EndFrame();
}

void TurnBaseSubsystem::PushState(ITurnState* s)
{
    printf("TurnBaseSubsystem::PushState     [Deferred] Push %s\n", s->m_turnStateName.c_str());
    m_pendingOps.push_back(PendingOp{PendingOp::Type::Push, s});
}

void TurnBaseSubsystem::PopState()
{
    m_pendingOps.push_back(PendingOp{PendingOp::Type::Pop, nullptr});
}

void TurnBaseSubsystem::UpdateState(float dt)
{
    /// Debug Print
    if (m_stateStack.size() > 0 && m_stateStack.back())
    {
        DebugAddMessage(Stringf("Current State: %s", m_stateStack.back()->m_turnStateName.c_str()), 0, Rgba8::RED, Rgba8::RED);
    }
    if (!m_stateStack.empty())
    {
        ITurnState* cur = m_stateStack.back();
        if (cur->GetIsInit() == false)
        {
            cur->OnEnter();
            cur->SetIsInit(true);
        }

        cur->Update(dt);

        if (cur->GetIsFinished())
        {
            cur->Exit(); // Exit does not move the stack, only marks itself
            // Pop & Push are handled uniformly below
            m_stateStack.pop_back();
            delete cur;
        }
    }

    // This must be processed unconditionally, and you cannot return just because the stack is empty!
    ProcessPendingOps();
}

bool TurnBaseSubsystem::GetStateStackEmptyExpectRound()
{
    if (dynamic_cast<StateRound*>(m_stateStack.at(0)) != nullptr && m_stateStack.size() == 1)
    {
        return true;
    }
    return false;
}

bool TurnBaseSubsystem::GetPendingPushEmpty()
{
    return m_pendingOps.empty();
}

ITurnState* TurnBaseSubsystem::GetStateStackBack()
{
    return m_stateStack.back();
}


void TurnBaseSubsystem::HandleTurnStateInput()
{
}

void TurnBaseSubsystem::HandleSubsystemInput()
{
    /// Debug Only
    if (g_theInput->WasKeyJustPressed(KEYCODE_LEFTBRACKET))
    {
        if (!m_bIsBattleStart)
        {
            m_bIsBattleStart = true;
            std::vector<std::string> yourCharacters;
            yourCharacters.push_back("Caizii");
            yourCharacters.push_back("Yellow");
            std::vector<std::string> enemyCharacters;
            enemyCharacters.push_back("CavelingBrute");
            enemyCharacters.push_back("Caveling");
            enemyCharacters.push_back("Caveling");
            StartBattle("SceneTest", yourCharacters, enemyCharacters);
            printf("TurnBaseSubsystem::HandleSubsystemInput     BattleStart\n");
            DebugAddMessage(Stringf("TurnBaseSubsystem::HandleSubsystemInput     BattleStart"), 5.0f);
        }
    }
    if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHTBRACKET))
    {
        if (m_bIsBattleStart)
        {
            printf("TurnBaseSubsystem::HandleSubsystemInput     BattleEnd\n");
            DebugAddMessage(Stringf("TurnBaseSubsystem::HandleSubsystemInput     BattleEnd"), 5.0f);
            ExitBattle();
        }
    }
}

void TurnBaseSubsystem::StartBattle(std::string sceneName, std::vector<std::string> yourCharacters, std::vector<std::string> enemyCharacters)
{
    printf("TurnBaseSubsystem::StartBattle      Turn-Base start in scene [%s] with:\n", sceneName.c_str());
    DebugAddMessage(Stringf("TurnBaseSubsystem::StartBattle      Turn-Base start in scene [%s] with:", sceneName.c_str()), 5.0f);
    std::string yours = "   ";
    for (unsigned int i = 0; i < yourCharacters.size(); i++)
    {
        yours.append(yourCharacters[i] + " ");
    }
    yours.append("| vs | ");
    for (unsigned int i = 0; i < enemyCharacters.size(); i++)
    {
        yours.append(enemyCharacters[i] + " ");
    }
    std::string debugLog = yours;
    yours.append("\n");
    printf("%s", yours.c_str());
    DebugAddMessage(Stringf(debugLog.c_str()), 5.0f);
    SavePlayerWorldData(g_theGame->GetLocalPlayer(1));
    LoadingBattleGround(sceneName);
    SetScenePlayerData(g_theGame->GetLocalPlayer(1));
    for (unsigned int i = 0; i < yourCharacters.size(); i++)
    {
        CharacterSlot slotTemp = {};
        slotTemp.m_fraction    = "Friend";
        slotTemp.m_slotIndex   = static_cast<int>(i);
        SpawnCharacterToSlot(yourCharacters[i], slotTemp);
    }
    for (unsigned int i = 0; i < enemyCharacters.size(); i++)
    {
        CharacterSlot slotTemp = {};
        slotTemp.m_fraction    = "Enemy";
        slotTemp.m_slotIndex   = static_cast<int>(i);
        SpawnCharacterToSlot(enemyCharacters[i], slotTemp);
    }
    /// Test Camera Director Framework
    CameraStateMetaData metaData = {};
    metaData.m_lengthOfDuration  = 8.0f;
    metaData.m_playbackType      = SpriteAnimPlaybackType::PING_PONG;
    metaData.m_targetPosition    = m_scene->m_definition->m_sceneCenter;
    metaData.m_targetFOV         = 60.f;
    metaData.m_targetRotation    = m_scene->m_definition->m_sceneCenterOrientation + Vec3(-5.f, 0.f, 0.f);

    m_cameraDirector->PushState(ICameraState(metaData, g_theGame->GetLocalPlayer(1)));
    PushState(new StateRound());
}

void TurnBaseSubsystem::ExitBattle()
{
    RetrievePlayerWorldData(g_theGame->GetLocalPlayer(1));
    m_bIsBattleStart = false;
    delete m_scene;
    m_scene = nullptr;
}

void TurnBaseSubsystem::LoadingBattleGround(std::string sceneName)
{
    printf("TurnBaseSubsystem::LoadingBattleGround      Loading battle ground scene [%s]\n", sceneName.c_str());
    DebugAddMessage(Stringf("TurnBaseSubsystem::LoadingBattleGround      Loading battle ground scene [%s]", sceneName.c_str()), 5.0f, Rgba8::ORANGE, Rgba8::ORANGE);
    const SceneDefinition* sceneDef = SceneDefinition::GetByName(sceneName);
    m_scene                         = new Scene(g_theGame, sceneDef);
    m_scene->m_renderContext        = g_theGame->GetLocalPlayer(1);
}

void TurnBaseSubsystem::SpawnCharacterToSlot(std::string character, const CharacterSlot& slot)
{
    for (const CharacterSlot& characterSlot : m_scene->m_definition->m_characterSlots)
    {
        if (characterSlot == slot)
        {
            SpawnInfo testInfo;
            testInfo.m_actorName   = character;
            testInfo.m_position    = characterSlot.m_slotCenter;
            testInfo.m_orientation = Vec3(characterSlot.m_slotOrientation);
            Character* c           = m_scene->SpawnCharacter(testInfo);
            c->SetSlot(characterSlot);
        }
    }
}


void TurnBaseSubsystem::SavePlayerWorldData(PlayerController* player)
{
    if (player == nullptr) return;
    m_savedCameraPos    = player->m_position;
    m_savedCameraRot    = player->m_orientation;
    m_savedPossessActor = player->GetActor();

    printf("TurnBaseSubsystem::SavePlayerCameraData      Save Camera Data from player: %d\n", player->m_index);
}

bool TurnBaseSubsystem::RetrievePlayerWorldData(PlayerController* player)
{
    if (player == nullptr) return false;
    player->m_position    = m_savedCameraPos;
    player->m_orientation = m_savedCameraRot;
    player->m_actorHandle = m_savedPossessActor->m_handle;
    printf("TurnBaseSubsystem::RetrievePlayerWorldData      Retrieve Camera Data to player: %d\n", player->m_index);
    return true;
}

void TurnBaseSubsystem::SetScenePlayerData(PlayerController* player)
{
    player->m_orientation = EulerAngles(m_scene->GetSceneDefinition()->m_sceneCenterOrientation);
    player->m_position    = m_scene->GetSceneDefinition()->m_sceneCenter;
    player->m_actorHandle = ActorHandle::INVALID;
    printf("TurnBaseSubsystem::SetScenePlayerCameraData      Set Scene Camera Data to player: %d\n", player->m_index);
}

bool TurnBaseSubsystem::GetIsInBattle()
{
    return m_bIsBattleStart;
}

Scene* TurnBaseSubsystem::GetCurrentScene()
{
    return m_scene;
}


void TurnBaseSubsystem::Startup()
{
    printf("TurnBaseSubsystem::Startup    Initialize Turn-Base Subsystem\n");
}

void TurnBaseSubsystem::Shutdown()
{
    printf("TurnBaseSubsystem::Shutdown     Shutdown Turn-Base Subsystem\n");
}

void TurnBaseSubsystem::Update()
{
    UpdateState(Clock::GetSystemClock().GetDeltaSeconds());
    if (m_scene)
    {
        m_cameraDirector->Update(Clock::GetSystemClock().GetDeltaSeconds());
        m_scene->Update(Clock::GetSystemClock().GetDeltaSeconds());
    }
    HandleSubsystemInput();
}

void TurnBaseSubsystem::Render()
{
    if (m_bIsBattleStart)
    {
        if (m_scene)
            m_scene->Render();
    }
}

void TurnBaseSubsystem::ProcessPendingOps()
{
    for (PendingOp& op : m_pendingOps)
    {
        if (op.type == PendingOp::Type::Push && op.state != nullptr)
        {
            m_stateStack.push_back(op.state);
            op.state->SetContext(this);
            op.state->OnInit();
        }
    }
    m_pendingOps.clear();
}
