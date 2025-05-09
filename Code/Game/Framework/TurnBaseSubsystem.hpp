#pragma once
#include <string>
#include <vector>

#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"

class WidgetTurnSequence;
class CameraDirector;
struct CharacterSlot;
class Actor;
class PlayerController;
class Scene;
class ITurnState;
class Character;

struct PendingOp
{
    enum class Type { Push, Pop };

    Type        type;
    ITurnState* state = nullptr;
};

struct InitiativeEntry
{
    Character* character; // Who
    int        score; // (可考虑 m_speed + d20)
};

struct TurnBaseSystemConfig
{
};

class TurnBaseSubsystem
{
public:
    TurnBaseSubsystem() = delete;
    TurnBaseSubsystem(TurnBaseSystemConfig config);
    ~TurnBaseSubsystem();

    void        EndFrame();
    void        PushState(ITurnState* s);
    void        PopState();
    void        UpdateState(float dt);
    bool        GetStateStackEmptyExpectRound(); // Except Round State
    bool        GetPendingPushEmpty();
    ITurnState* GetStateStackBack();
    void        HandleTurnStateInput(); // 只转发给栈顶
    void        HandleSubsystemInput();

    void StartBattle(std::string sceneName, std::vector<std::string> yourCharacters, std::vector<std::string> enemyCharacters);
    void ExitBattle();
    void LoadingBattleGround(std::string sceneName);
    void SpawnCharacterToSlot(std::string character, const CharacterSlot& slot);

    /// Map To Scene Data flow Management
    void SavePlayerWorldData(PlayerController* player);
    bool RetrievePlayerWorldData(PlayerController* player);
    void SetScenePlayerData(PlayerController* player); // Temporary, we need push the Root state that handle camera reset and hanging to stack

    /// Getter
    bool                GetIsInBattle();
    Scene*              GetCurrentScene();
    WidgetTurnSequence* GetWidgetTurnSequence() { return m_widgetTurnSequence; }

    void Startup();
    void Shutdown();
    void Update();
    void Render();

    CameraDirector* m_cameraDirector = nullptr;

private:
    void ProcessPendingOps();

    TurnBaseSystemConfig         m_config;
    std::vector<InitiativeEntry> m_roundTimeline;
    std::vector<ITurnState*>     m_stateStack;
    std::vector<PendingOp>       m_pendingOps;
    Scene*                       m_scene = nullptr; // Battle Scene

    Actor* m_savedPossessActor = nullptr;


    /// Camera
    Vec3        m_savedCameraPos; // We want player camera back correctly when battle end.
    EulerAngles m_savedCameraRot;

    /// Debug Testing
    bool m_bIsBattleStart = false;

    /// Widget
    WidgetTurnSequence* m_widgetTurnSequence = nullptr;
};
