#pragma once
#include "Game/Definition/SceneDefinition.hpp"
#include "Game/Gameplay/Actor.hpp"

class WidgetCharacterStateHud;
struct DamageData;
enum class BuffTag;
struct Buff;
class ISkill;
class AnimationDefinition;
class CharacterDefinition;
class Animation;
class Scene;

///
/// Character tha will take part in battle.
/// 
class Character
{
    friend class Scene;
    friend struct Buff;

public:
    Character(const SpawnInfo& spawnInfo);
    virtual ~Character();

    void TakeTurn();
    void ReceiveDamage(DamageData inDamageData);

    void         PostInitialize();
    void         Update(float deltaSeconds);
    void         HandleCharacterDeath();
    void         UpdatePhysics(float deltaSeconds);
    void         AddForce(Vec3 force);
    void         MoveToPosition(Vec3 newPosition);
    void         MoveInDirection(Vec3 direction, float speed);
    virtual void TurnInDirection(Vec3 direction);

    void  Render() const;
    Mat44 GetModelToWorldTransform() const;

    void InitLocalVertex();

    void                 UpdateAnimation(float deltaSeconds);
    AnimationDefinition* PlayAnimationByState(std::string animationState, bool force = false);

    /// Setter
    void        SetSlot(CharacterSlot slot);
    Controller* SetCharacterController(Controller* inController);
    bool        SetCharacterDead(bool newDead, float delayAfterMarkAsGarbage);

    /// Getter
    CharacterDefinition* GetCharacterDefinition() const { return m_definition; }
    Vec3                 GetPosition() const { return m_position; }
    Controller*          GetController() const { return m_controller; }
    std::vector<ISkill*> GetSkills() const { return m_skills; }
    int                  GetCurrentHealth() const { return static_cast<int>(m_currentHealth); }
    int                  GetCurrentMana() const { return static_cast<int>(m_currentMana); }
    int                  GetMaxMana() const { return static_cast<int>(m_maxMana); }
    bool                 GetIsGarbage() const { return m_bIsGarbage; }
    ActorHandle          GetHandle() const { return m_handle; }
    /// Buff
    void AddBuff(Buff& newb);
    bool HasBuff(const Buff& b);
    bool HasBuff(const BuffTag& tag);
    void OnRoundStart(); // 处理Break等逻辑
    void OnRoundEnd(); // 扣回合，清理已过期
    /// Properties
    int  RollInitiative() const;
    bool GetIsDead() const;
    bool CanAct() const;
    int  GetUniqueID();

    CharacterSlot GetSlot() const;

protected:
    /// Properties In RPG
    float                m_currentHealth = 0.f;
    float                m_currentMana   = 100.f;
    float                m_maxMana       = 100.f;
    float                m_maxHealth     = 0.f;
    int                  m_baseSpeed     = 10; // 固定属性，可被装备/等级成长
    int                  shieldPoint     = 2; // 破防护盾
    bool                 m_canAct        = true;
    bool                 m_isDead        = false;
    std::vector<ISkill*> m_skills;
    std::vector<Buff>    m_buffs;

    EulerAngles m_orientation; // 3D orientation, as EulerAngles, in degrees.
    Vec3        m_velocity; // 3D velocity, as a Vec3, in world units per second.
    Vec3        m_acceleration;

    bool  m_bIsStatic;
    float m_physicalHeight;
    float m_physicalRadius;

    Scene*      m_scene      = nullptr;
    ActorHandle m_handle     = ActorHandle::INVALID; // This actor's handle, assigned by the map when this actor was spawned.
    bool        m_bIsGarbage = false;
    Timer*      m_deathTimer = nullptr;

    Vec3 m_position = Vec3::ZERO; // In Scene

    Timer*               m_animationTimer          = nullptr;
    AnimationDefinition* m_currentPlayingAnimation = nullptr;
    CharacterDefinition* m_definition              = nullptr;

    std::vector<Vertex_PCU> m_vertexes;
    Rgba8                   m_color = Rgba8::WHITE;

    CharacterSlot m_slot; // Slot for occupation

    /// Controller and AI
    Controller* m_controller = nullptr;

    /// Render Specific
    Vec2 m_spriteSize;

    /// Hud
    WidgetCharacterStateHud* m_hud = nullptr;
};
