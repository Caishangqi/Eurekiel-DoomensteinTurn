#include "AIController.hpp"

#include "TurnBaseSubsystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/ActorDefinition.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Definition/WeaponDefinition.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/Weapon.hpp"
#include "Game/Gameplay/Battle/Scene.hpp"
#include "Game/Gameplay/Battle/Skill/ISkill.hpp"
#include "Game/Gameplay/Battle/State/StateSkillExecution.hpp"

AIController::AIController(Map* map): Controller(map)
{
}

void AIController::Update(float deltaTime)
{
    Controller::Update(deltaTime);
    m_state                = "None";
    Actor* controlledActor = m_map->GetActorByHandle(m_actorHandle);
    if (!controlledActor || controlledActor->m_bIsDead)
    {
        return;
    }
    Actor* target = m_map->GetClosestVisibleEnemy(controlledActor);
    if (target && m_targetActorHandle.IsValid() && m_targetActorHandle != target->m_handle && !target->m_bIsDead)
    {
        m_targetActorHandle = target->m_handle;
        printf("AIController::Update > Target actor changed to %s\n", target->m_definition->m_name.c_str());
    }
    Actor* targetActor = m_map->GetActorByHandle(m_targetActorHandle);
    if (!targetActor || targetActor->m_bIsDead)
    {
        m_targetActorHandle = ActorHandle::INVALID;
        return;
    }

    float turnSpeedDegPerSec      = controlledActor->m_definition->m_turnSpeed;
    float maxTurnDegreesThisFrame = turnSpeedDegPerSec * deltaTime;
    Vec3  toTarget3D              = targetActor->m_position - controlledActor->m_position;
    toTarget3D.z                  = 0.f;

    float desiredYaw = Atan2Degrees(toTarget3D.y, toTarget3D.x);
    float currentYaw = controlledActor->m_orientation.m_yawDegrees;
    float newYaw     = GetTurnedTowardDegrees(currentYaw, desiredYaw, maxTurnDegreesThisFrame);

    auto newDirectionTurningTo = Vec3(newYaw, controlledActor->m_orientation.m_pitchDegrees, controlledActor->m_orientation.m_rollDegrees);
    controlledActor->TurnInDirection(newDirectionTurningTo);
    float distanceToTarget = toTarget3D.GetLength();
    float combinedRadius   = controlledActor->m_physicalRadius + targetActor->m_physicalRadius;
    if (distanceToTarget > combinedRadius)
    {
        float moveSpeed = controlledActor->m_definition->m_runSpeed;
        Vec3  forward, left, up;
        controlledActor->m_orientation.GetAsVectors_IFwd_JLeft_KUp(forward, left, up);
        controlledActor->MoveInDirection(forward, moveSpeed);
        controlledActor->PlayAnimationByName("Walk");
    }
    /// Hanlde melee weapon based on melee weapon range
    if (controlledActor->m_currentWeapon && controlledActor->m_currentWeapon->m_definition->m_meleeCount > 0)
    {
        /// 0.2f ensure that AI try attack out side of meel range give player more opportunity.
        if (distanceToTarget < controlledActor->m_currentWeapon->m_definition->m_meleeRange + targetActor->m_physicalRadius + 0.2f)
        {
            controlledActor->m_currentWeapon->Fire();
            controlledActor->PlayAnimationByName(m_state, true);
        }
    }
    /*else
    {
        controlledActor->m_currentWeapon->Fire();
        // controlledActor->MoveInDirection(dirToTarget, moveSpeed * 0.2f);
    }*/
}

void AIController::Possess(ActorHandle& actorHandle)
{
    Controller::Possess(actorHandle);
}

Actor* AIController::GetActor()
{
    return Controller::GetActor();
}

void AIController::DamagedBy(ActorHandle& attacker)
{
    m_targetActorHandle = attacker;
    printf("AIController::DamagedBy    > Target actor changed to %s\n", m_map->GetActorByHandle(attacker)->m_definition->m_name.c_str());
}

void AIController::HandleSkillSelection(Character* character)
{
    printf("AIController::HandleSkillSelection      Character [%s] handling skill selection.\n", character->GetCharacterDefinition()->m_name.c_str());

    std::vector<Character*> potentialTargets = g_theGame->m_turnBaseSubsystem->GetCurrentScene()->GetCharactersByFraction("Friend");
    if (potentialTargets.size() == 0)
        return;
    int targetIndex = g_rng->RollRandomIntInRange(0, static_cast<int>(potentialTargets.size()) - 1);
    if (character->GetCharacterDefinition()->m_name == "CavelingBrute")
    {
        ISkill* skill0 = character->GetSkills()[0]->Duplicate();
        skill0->AddToTarget(potentialTargets[targetIndex]);
        auto stateExecution = new StateSkillExecution(skill0, character);
        g_theGame->m_turnBaseSubsystem->PushState(stateExecution);
    }
    if (character->GetCharacterDefinition()->m_name == "Caveling")
    {
        ISkill* skill0 = character->GetSkills()[0]->Duplicate();
        skill0->AddToTarget(potentialTargets[targetIndex]);
        auto stateExecution = new StateSkillExecution(skill0, character);
        g_theGame->m_turnBaseSubsystem->PushState(stateExecution);
    }
}
