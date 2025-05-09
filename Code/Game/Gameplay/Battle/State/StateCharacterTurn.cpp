#include "StateCharacterTurn.hpp"

#include "StateMoveToSlot.hpp"
#include "StateSkillSelection.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Battle/Scene.hpp"
#include "Game/Gameplay/Widget/WidgetTurnSequence.hpp"

StateCharacterTurn::StateCharacterTurn(Character* character): m_character(character)
{
    m_turnStateName = "StateCharacterTurn";
}

StateCharacterTurn::~StateCharacterTurn()
{
}

void StateCharacterTurn::Update(float dt)
{
    UNUSED(dt)
}

bool StateCharacterTurn::GetIsFinished() const
{
    return m_isFinished;
}

void StateCharacterTurn::OnInit()
{
    printf("StateCharacterTurn::OnInit       Character State OnInit, owner: %s\n", m_character->GetCharacterDefinition()->m_name.c_str());
    m_turnBaseSubsystem->GetWidgetTurnSequence()->InjectCharacterTurnState(this);
}

void StateCharacterTurn::OnEnter()
{
    ITurnState::OnEnter();
    m_turnBaseSubsystem->PushState(new StateSkillSelection(m_character));
    if (m_character->GetCharacterDefinition()->m_faction == "Friend")
    {
        m_turnBaseSubsystem->PushState(new StateMoveToSlot(m_character, *m_turnBaseSubsystem->GetCurrentScene()->GetStageSlot()));
    }
    m_isFinished = true;
}

void StateCharacterTurn::Exit()
{
}

bool StateCharacterTurn::GetCharacterTurnOver()
{
    return m_bCharacterTurnOver;
}
