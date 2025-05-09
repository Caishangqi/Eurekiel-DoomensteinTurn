#include "Character.hpp"

#include "Battle/Scene.hpp"
#include "Battle/Buff/Buff.hpp"
#include "Battle/Buff/BuffTag.hpp"
#include "Battle/Skill/ISkill.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/ActorDefinition.hpp"
#include "Game/Definition/AnimationDefinition.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/AIController.hpp"
#include "Game/Framework/Animation.hpp"
#include "Game/Framework/PlayerController.hpp"
#include "Game/Framework/WidgetSubsystem.hpp"
#include "Widget/WidgetCharacterStateHud.hpp"
#include "Widget/WidgetDamageIndicator.hpp"

Character::Character(const SpawnInfo& spawnInfo)
{
    CharacterDefinition* definition = CharacterDefinition::GetByName(spawnInfo.m_actorName);
    if (definition == nullptr)
    {
        ERROR_AND_DIE(Stringf("CharacterDefinition::CharacterDefinition    - Character definition not found for name \"%s\".\n", spawnInfo.m_actorName.c_str()));
    }
    m_definition              = definition;
    m_physicalHeight          = definition->m_physicsHeight;
    m_currentHealth           = definition->m_health;
    m_maxHealth               = m_currentHealth;
    m_maxMana                 = definition->m_maxMana;
    m_currentMana             = m_maxMana;
    m_physicalRadius          = definition->m_physicsRadius;
    m_position                = spawnInfo.m_position;
    m_velocity                = spawnInfo.m_velocity;
    m_orientation             = EulerAngles(spawnInfo.m_orientation);
    m_currentPlayingAnimation = m_definition->m_defaultAnimation;
    m_skills                  = m_definition->m_skills;
    m_baseSpeed               = m_definition->m_baseSpeed;
    m_spriteSize              = m_definition->m_size;
    // TODO: Dangerous, Caizii set the definition skill owner, it should not happen!
    // Consider use Duplicated method.
    for (ISkill* skill : m_skills)
    {
        skill->SetOwner(this);
    }

    /// Initialize Controller
    if (m_definition->m_faction == "Friend")
        SetCharacterController(g_theGame->GetLocalPlayer(1));
    /// Initialize AI COntroller
    if (m_definition->m_faction == "Enemy")
        SetCharacterController(new AIController(nullptr));
    InitLocalVertex();
    printf("Object::Character    + Creating Character at (%f, %f, %f)\n", m_position.x, m_position.y, m_position.z);
}

Character::~Character()
{
    if (dynamic_cast<AIController*>(m_controller))
    {
        delete m_controller;
        m_controller = nullptr;
    }
    else
    {
        m_controller = nullptr;
    }

    m_scene = nullptr;

    delete m_animationTimer;
    m_animationTimer = nullptr;

    delete m_deathTimer;
    m_deathTimer = nullptr;

    m_hud->RemoveFromViewport();
}

void Character::TakeTurn()
{
}

void Character::ReceiveDamage(DamageData inDamageData)
{
    printf("Character::ReceiveDamage        Character [%s] received [%d] damage, instigator [%s]\n", m_definition->m_name.c_str(), inDamageData.m_damage,
           inDamageData.m_instigator->m_definition->m_name.c_str());
    m_currentHealth -= inDamageData.m_damage;
    PlayAnimationByState("Hurt");
    printf("Character::ReceiveDamage        Character [%s] health [%d / %d]\n", m_definition->m_name.c_str(), static_cast<int>(m_currentHealth), static_cast<int>(m_maxHealth));
    if (m_currentHealth <= 0)
    {
        SetCharacterDead(true, m_definition->GetAnimationByState("Dead")->GetAnimationLength());
        PlayAnimationByState("Dead", true);
        printf("Character::ReceiveDamage        Character [%s] is dead\n", m_definition->m_name.c_str());
    }

    /// Damage Indicator
    auto damageIndicator            = new WidgetDamageIndicator();
    damageIndicator->m_position     = m_position + Vec3(0.f, 0.f, m_definition->m_size.y * 0.5f);
    damageIndicator->m_data         = std::to_string(inDamageData.m_damage);
    damageIndicator->m_bInterpolate = true;
    damageIndicator->m_startColor   = Rgba8::RED;
    damageIndicator->m_endColor     = Rgba8::ORANGE;
    damageIndicator->m_size         = m_definition->m_size.y * 100.f;
    damageIndicator->m_dimensions   = Vec2(300.f, 200.f);
    g_theWidgetSubsystem->AddToViewport(damageIndicator);
}


void Character::PostInitialize()
{
    m_animationTimer = new Timer(0, g_theGame->m_clock); // Create timer
    m_animationTimer->Start();
}

void Character::Update(float deltaSeconds)
{
    UpdateAnimation(deltaSeconds);
    UpdatePhysics(deltaSeconds);
    HandleCharacterDeath();
}

void Character::HandleCharacterDeath()
{
    if (m_isDead && m_deathTimer->HasPeriodElapsed())
    {
        m_bIsGarbage = true;
    }
}

void Character::UpdatePhysics(float deltaSeconds)
{
    float dragValue = m_definition->m_drag;
    Vec3  dragForce = -m_velocity * dragValue;
    AddForce(dragForce);

    m_velocity += m_acceleration * deltaSeconds;
    m_position += m_velocity * deltaSeconds;

    if (!m_definition->m_flying)
    {
        m_position.z = 0.f;
    }
    m_acceleration = Vec3::ZERO;
}

void Character::AddForce(Vec3 force)
{
    m_acceleration += force;
}

void Character::MoveToPosition(Vec3 newPosition)
{
    m_position = newPosition;
}

void Character::MoveInDirection(Vec3 direction, float speed)
{
    Vec3 dirNormalized = direction.GetNormalized();
    // 'speed' as velocity while overcoming drag
    float dragValue = m_definition->m_drag;
    Vec3  force     = dirNormalized * (speed * dragValue);
    AddForce(force);
}

void Character::TurnInDirection(Vec3 direction)
{
    m_orientation = EulerAngles(direction);
}

void Character::Render() const
{
    Mat44 localToWorldMat;
    if (m_definition->m_billboardType == "None")
        localToWorldMat = GetModelToWorldTransform();
    else
    {
        if (m_definition->m_billboardType == "WorldUpFacing")
        {
            Mat44 cameraTransform = Mat44::MakeTranslation3D(m_scene->m_renderContext->m_position);
            cameraTransform.Append(m_scene->m_renderContext->m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
            localToWorldMat = Mat44::MakeTranslation3D(m_position);
            localToWorldMat.Append(GetBillboardTransform(BillboardType::WORLD_UP_FACING, cameraTransform, m_position));
        }
        else if (m_definition->m_billboardType == "FullOpposing")
        {
            Mat44 cameraTransform = Mat44::MakeTranslation3D(m_scene->m_renderContext->m_position);
            cameraTransform.Append(m_scene->m_renderContext->m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
            localToWorldMat.Append(GetBillboardTransform(BillboardType::FULL_OPPOSING, cameraTransform, m_position));
        }
        else if (m_definition->m_billboardType == "WorldUpOpposing")
        {
            Mat44 cameraTransform = Mat44::MakeTranslation3D(m_scene->m_renderContext->m_position);
            cameraTransform.Append(m_scene->m_renderContext->m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
            localToWorldMat.Append(GetBillboardTransform(BillboardType::WORLD_UP_OPPOSING, cameraTransform, m_position));
        }
        else
        {
            localToWorldMat = GetModelToWorldTransform();
        }
    }
    const SpriteAnimDefinition* anim         = m_currentPlayingAnimation->GetAnimationDefinition();
    const SpriteDefinition      spriteAtTime = anim->GetSpriteDefAtTime(m_animationTimer->GetElapsedTime() * 1); // TODO: Handle animation speed.
    AABB2                       uvAtTime     = spriteAtTime.GetUVs();
    Vec2                        spriteOffSet = -m_spriteSize * m_definition->m_pivot;
    auto                        bottomLeft   = Vec3(0, spriteOffSet.x, spriteOffSet.y);
    Vec3                        bottomRight  = bottomLeft + Vec3(0, m_spriteSize.x, 0);
    Vec3                        topLeft      = bottomLeft + Vec3(0, 0, m_spriteSize.y);
    Vec3                        topRight     = bottomRight + Vec3(0, 0, m_spriteSize.y);

    /// Create geometry.
    bool                       bIsLit = m_definition->m_renderLit;
    std::vector<Vertex_PCUTBN> vertexesLit;
    std::vector<Vertex_PCU>    vertexesUnlit;
    if (bIsLit)
    {
        if (m_definition->m_renderRounded)
        {
            vertexesLit.reserve(8192);
            AddVertsForRoundedQuad3D(vertexesLit, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, uvAtTime);
        }
        else
        {
            vertexesUnlit.reserve(8192);
            AddVertsForQuad3D(vertexesLit, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, uvAtTime);
        }
        g_theRenderer->SetModelConstants(localToWorldMat, Rgba8::WHITE);
        g_theRenderer->BindShader(m_definition->m_shader);
        g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
        g_theRenderer->SetLightConstants(m_scene->m_sunDirection, m_scene->m_sunIntensity, m_scene->m_ambientIntensity);
        g_theRenderer->BindTexture(&spriteAtTime.GetTexture());
        g_theRenderer->DrawVertexArray(vertexesLit);
        return;
    }

    if (!bIsLit)
    {
        vertexesUnlit.reserve(8192);
        AddVertsForQuad3D(vertexesUnlit, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, uvAtTime);
        g_theRenderer->SetModelConstants(localToWorldMat, Rgba8::WHITE);
        g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
        g_theRenderer->BindShader(m_definition->m_shader);
        g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
        g_theRenderer->BindTexture(&spriteAtTime.GetTexture());
        ///g_theRenderer->BindTexture(nullptr);
        g_theRenderer->DrawVertexArray(vertexesUnlit);
        g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
        return;
    }

    g_theRenderer->SetModelConstants(localToWorldMat, m_color);
    g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
    Texture* currentAnimTexture = &m_currentPlayingAnimation->GetAnimationSpriteSheet()->GetTexture();
    g_theRenderer->BindTexture(currentAnimTexture);
    g_theRenderer->DrawVertexArray(m_vertexes);
}

Mat44 Character::GetModelToWorldTransform() const
{
    Mat44       matTranslation = Mat44::MakeTranslation3D(m_position);
    EulerAngles orientationRender;
    orientationRender.m_yawDegrees = m_orientation.m_yawDegrees;
    matTranslation.Append(orientationRender.GetAsMatrix_IFwd_JLeft_KUp());
    return matTranslation;
}

void Character::InitLocalVertex()
{
    m_vertexes.reserve(8192);
}

void Character::UpdateAnimation(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (!m_currentPlayingAnimation)
    {
        m_currentPlayingAnimation = m_definition->m_defaultAnimation;
        m_spriteSize              = m_currentPlayingAnimation->GetSpriteWorldDimension() == Vec2::INVALID ? m_definition->m_size : m_currentPlayingAnimation->GetSpriteWorldDimension();
    }

    if (m_animationTimer->GetElapsedTime() > m_currentPlayingAnimation->GetAnimationLength())
    {
        if (m_currentPlayingAnimation->GetAnimationPlaybackType() == SpriteAnimPlaybackType::LOOP)
        {
            return;
        }
        m_currentPlayingAnimation = m_definition->GetAnimationByState("Idle");
        m_spriteSize              = m_currentPlayingAnimation->GetSpriteWorldDimension() == Vec2::INVALID ? m_definition->m_size : m_currentPlayingAnimation->GetSpriteWorldDimension();
        m_animationTimer->Stop();
        m_animationTimer->Start();
    }
}

AnimationDefinition* Character::PlayAnimationByState(std::string animationState, bool force)
{
    AnimationDefinition* foundedAnimation = m_definition->GetAnimationByState(animationState);
    if (foundedAnimation)
    {
        if (foundedAnimation == m_currentPlayingAnimation)
        {
            return foundedAnimation;
        }
        /// We want to replace to new animation, force update it whether or not it finished
        if (force)
        {
            m_currentPlayingAnimation = foundedAnimation;
            m_spriteSize              = foundedAnimation->GetSpriteWorldDimension() == Vec2::INVALID ? m_definition->m_size : foundedAnimation->GetSpriteWorldDimension();
            m_animationTimer->Start();
            return foundedAnimation;
        }
        if (m_currentPlayingAnimation)
        {
            bool isCurrentAnimFinished = m_animationTimer->GetElapsedTime() >= m_currentPlayingAnimation->GetAnimationLength();
            if (isCurrentAnimFinished)
            {
                m_currentPlayingAnimation = foundedAnimation;
                m_spriteSize              = foundedAnimation->GetSpriteWorldDimension() == Vec2::INVALID ? m_definition->m_size : foundedAnimation->GetSpriteWorldDimension();
                m_animationTimer->Start();
                return foundedAnimation;
            }
        }
        else
        {
            m_currentPlayingAnimation = foundedAnimation;
            m_spriteSize              = foundedAnimation->GetSpriteWorldDimension() == Vec2::INVALID ? m_definition->m_size : foundedAnimation->GetSpriteWorldDimension();
            m_animationTimer->Start();
            return foundedAnimation;
        }
    }
    return nullptr;
}

void Character::SetSlot(CharacterSlot slot)
{
    m_slot = slot;
}

Controller* Character::SetCharacterController(Controller* inController)
{
    m_controller = inController;
    return m_controller;
}

bool Character::SetCharacterDead(bool newDead, float delayAfterMarkAsGarbage)
{
    m_isDead = newDead;
    if (m_deathTimer)
    {
        delete m_deathTimer;
        m_deathTimer = nullptr;
    }
    m_deathTimer = new Timer(delayAfterMarkAsGarbage, g_theGame->m_clock);
    m_deathTimer->Start();
    return newDead;
}


void Character::AddBuff(Buff& newb)
{
    for (Buff& b : m_buffs)
    {
        if (b.m_tag == newb.m_tag && !b.m_stackable)
        {
            b.m_turnsLeft = newb.m_turnsLeft;
            return;
        }
    }
    m_buffs.push_back(newb);
    newb.m_owner = this;
    printf("Character::AddBuff      Add buff: %s \n", newb.ToString().c_str());
    m_buffs.back().OnInit();
}

bool Character::HasBuff(const Buff& b)
{
    for (const Buff& buff : m_buffs)
    {
        if (buff.m_tag == b.m_tag)
        {
            return true;
        }
    }
    return false;
}

bool Character::HasBuff(const BuffTag& tag)
{
    for (const Buff& buff : m_buffs)
    {
        if (buff.m_tag == tag)
        {
            return true;
        }
    }
    return false;
}

void Character::OnRoundStart()
{
    m_canAct = true; // 每轮先置位

    /// Hud Creation
    if (!m_hud)
    {
        m_hud = new WidgetCharacterStateHud(this);
        m_hud->SetCreateHealthIndicator(m_definition->m_faction == "Friend"); // We create health bar for friend faction
        g_theWidgetSubsystem->AddToViewport(m_hud);
    }

    for (Buff& b : m_buffs)
    {
        b.OnRoundStart(); // 让 Buff 决定是否禁用行动
    }
}

void Character::OnRoundEnd()
{
    for (Buff& b : m_buffs)
        b.OnRoundEnd();
    m_buffs.erase(std::remove_if(m_buffs.begin(), m_buffs.end(), [](const Buff& b) { return b.m_turnsLeft == 0; }), m_buffs.end());
}

int Character::RollInitiative() const
{
    int speedBonus = 0;
    for (const Buff& b : m_buffs)
    {
        if (b.m_tag == BuffTag::Speed) speedBonus += b.m_value;
        if (b.m_tag == BuffTag::Break) return 0; // 破防强行 0
    }
    return m_baseSpeed + speedBonus + g_rng->RollRandomIntInRange(0, 6);
}

bool Character::GetIsDead() const
{
    return m_isDead;
}

bool Character::CanAct() const
{
    return m_canAct;
}

int Character::GetUniqueID()
{
    return static_cast<int>(m_handle.GetData());
}

CharacterSlot Character::GetSlot() const
{
    return m_slot;
}
