#include "StateCharacterLeave.hpp"

StateCharacterLeave::StateCharacterLeave(Character* character): m_character(character)
{
}

StateCharacterLeave::~StateCharacterLeave()
{
    delete m_timer;
    m_timer = nullptr;
}

void StateCharacterLeave::OnInit()
{
}

void StateCharacterLeave::OnEnter()
{
    ITurnState::OnEnter();
}

void StateCharacterLeave::Update(float dt)
{
}

void StateCharacterLeave::Exit()
{
}
