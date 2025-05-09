#include "CameraDirector.hpp"

void CameraDirector::PushState(ICameraState state)
{
    m_stack.push_back(state);
    state.Enter();
}

void CameraDirector::PopState()
{
    if (!m_stack.empty())
    {
        m_stack.back().Exit();
        m_stack.pop_back();
    }
}

void CameraDirector::Update(float deltaTime)
{
    if (!m_stack.empty())
    {
        m_stack.back().Update(deltaTime);
        if (m_stack.back().GetIsFinished())
        {
        }
    }
}
