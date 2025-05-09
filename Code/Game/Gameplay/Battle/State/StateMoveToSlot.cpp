#include "StateMoveToSlot.hpp"

#include "StateSkillSelection.hpp"
#include "Engine/Core/Timer.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"

StateMoveToSlot::StateMoveToSlot(Character* character, CharacterSlot characterSlot): m_character(character), m_targetSlot(characterSlot)
{
    m_turnStateName = "StateMoveToSlot";
    m_timer         = new Timer(1.0f, g_theGame->m_clock);
    m_timer->Stop();
}

StateMoveToSlot::~StateMoveToSlot()
{
    delete m_timer;
    m_timer = nullptr;
}

void StateMoveToSlot::OnInit()
{
    Vec3 characterPos = m_character->GetPosition();
    printf("StateMoveToSlot::OnInit       StateMoveToSlot OnInit, owner: %s\n"
           "            Target Position: (%f,%f,%f)\n"
           "            Current Position (%f,%f,%f)\n", m_character->GetCharacterDefinition()->m_name.c_str(), m_targetSlot.m_slotCenter.x, m_targetSlot.m_slotCenter.y, m_targetSlot.m_slotCenter.z,
           characterPos.x, characterPos.y, characterPos.z);
    m_timer->Start();
}

void StateMoveToSlot::OnEnter()
{
    ITurnState::OnEnter();
}

void StateMoveToSlot::Update(float dt)
{
    UNUSED(dt)
    float dist = GetDistanceSquared2D(m_character->GetPosition().GetXY(), m_targetSlot.m_slotCenter.GetXY());
    if (dist < 0.01f)
    {
        m_isFinished   = true;
        Vec3 playerPos = m_character->GetPosition();
        printf("StateMoveToSlot::Update     Reach Desired Position [%.1f,%.1f,%.1f]\n", playerPos.x, playerPos.y, playerPos.z);
        return;
    }
    float fraction       = m_timer->GetElapsedFraction();
    Vec3  interpolatePos = Interpolate(m_character->GetPosition(), m_targetSlot.m_slotCenter, fraction);
    //printf("StateMoveToSlot::Update     Moving to Position [%.1f,%.1f,%.1f]\n", interpolatePos.x, interpolatePos.y, interpolatePos.z);
    m_character->MoveToPosition(interpolatePos);
}

void StateMoveToSlot::Exit()
{
}
