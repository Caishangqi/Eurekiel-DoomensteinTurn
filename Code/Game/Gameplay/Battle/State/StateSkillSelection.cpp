#include "StateSkillSelection.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/AIController.hpp"
#include "Game/Framework/PlayerController.hpp"
#include "Game/Framework/WidgetSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Widget/WidgetSkillSelection.hpp"

StateSkillSelection::StateSkillSelection(Character* character): m_character(character)
{
    m_turnStateName = "StateSkillSelection";
}

StateSkillSelection::~StateSkillSelection()
{
}

void StateSkillSelection::OnEnter()
{
    ITurnState::OnEnter();
    printf("StateSkillSelection::OnEnter       StateSkillSelection OnEnter, owner: %s\n", m_character->GetCharacterDefinition()->m_name.c_str());

    /// If Is Player We need display skill selection widget
    if (dynamic_cast<PlayerController*>(m_character->GetController()))
    {
        m_widgetSkillSelection = new WidgetSkillSelection(m_character, this);
        g_theWidgetSubsystem->AddToPlayerViewport(m_widgetSkillSelection, dynamic_cast<PlayerController*>(m_character->GetController()), 0);
        m_widgetSkillSelection->SetUserFocus();
    }

    if (dynamic_cast<AIController*>(m_character->GetController()))
    {
        auto aiController = dynamic_cast<AIController*>(m_character->GetController());
        aiController->HandleSkillSelection(m_character);
        m_isFinished = true;
    }
}

void StateSkillSelection::OnInit()
{
}

void StateSkillSelection::Update(float dt)
{
    UNUSED(dt)
}

void StateSkillSelection::Exit()
{
    if (m_widgetSkillSelection)
    {
        g_theWidgetSubsystem->RemoveFromViewport(m_widgetSkillSelection);
    }
}

bool StateSkillSelection::GetIsFinished() const
{
    return m_isFinished;
}

Character* StateSkillSelection::GetCharacter() const
{
    return m_character;
}
