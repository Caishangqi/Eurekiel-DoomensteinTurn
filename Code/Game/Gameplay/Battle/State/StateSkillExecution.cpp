#include "StateSkillExecution.hpp"

#include "StateMoveToSlot.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Battle/Scene.hpp"
#include "Game/Gameplay/Battle/Skill/ISkill.hpp"

StateSkillExecution::StateSkillExecution(ISkill* inExecuteSkill, Character* inCharacter): m_executeSkill(inExecuteSkill), m_character(inCharacter)
{
    m_turnStateName       = "StateSkillExecution";
    m_animationDefinition = m_executeSkill->GetStartAnimation();
    m_animationLength     = m_animationDefinition->GetAnimationLength();
    m_animationTimer      = new Timer(m_animationLength, g_theGame->m_clock);
    m_animationName       = m_executeSkill->GetStartAnimationName();
    m_animationTimer->Stop();
    m_executeSkill = inExecuteSkill->Duplicate(); // We need duplicate the skill for execution or else the target pool will contaminate
}

void StateSkillExecution::OnInit()
{
}

void StateSkillExecution::OnEnter()
{
    ITurnState::OnEnter();
    Character* primarySelection = m_turnBaseSubsystem->GetCurrentScene()->GetCharacterByHandle(m_executeSkill->GetTargets().front()->GetHandle());
    if (primarySelection && m_executeSkill->GetMoveToTarget())
    {
        CharacterSlot newSlot = primarySelection->GetSlot();
        newSlot.m_slotCenter += m_executeSkill->GetMoveToOffset();
        m_turnBaseSubsystem->PushState(new StateMoveToSlot(m_character, newSlot));
    }
    m_character->PlayAnimationByState(m_animationName, true);
    m_animationTimer->Start();
}

void StateSkillExecution::Update(float dt)
{
    UNUSED(dt)
    if (!m_turnBaseSubsystem->GetCurrentScene()->GetCharacterByHandle(m_character->GetHandle()))
    {
        m_isFinished = true;
        return;
    }
    if (m_animationTimer->GetElapsedTime() >= m_animationLength)
    {
        m_animationTimer->Stop();
        m_executeSkill->StartAction(m_character);
        g_theGame->m_turnBaseSubsystem->PushState(new StateMoveToSlot(m_character, m_character->GetSlot()));
        m_isFinished = true;
    }
}

void StateSkillExecution::Exit()
{
    m_executeSkill->ClearTargets(); // We clean Targets inside the duplicated skills
    delete m_executeSkill;
    m_executeSkill = nullptr;
}
