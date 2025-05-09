#include "ITurnState.hpp"

bool ITurnState::SetContext(TurnBaseSubsystem* sys)
{
    m_turnBaseSubsystem = sys;
    if (!m_turnBaseSubsystem)
        return false;
    return true;
}

void ITurnState::OnEnter()
{
    printf("ITurnState::OnEnter       %s State OnInit\n", m_turnStateName.c_str());
}

bool ITurnState::GetIsFinished() const
{
    return m_isFinished;
}

bool ITurnState::GetIsInit() const
{
    return m_bIsInit;
}

bool ITurnState::SetIsInit(bool value)
{
    m_bIsInit = value;
    return m_bIsInit;
}

bool ITurnState::SetIsFinished(bool value)
{
    m_isFinished = value;
    return value;
}
