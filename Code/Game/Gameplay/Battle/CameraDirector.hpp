#pragma once
#include <vector>

#include "State/ICameraState.hpp"

class CameraDirector
{
public:
    void PushState(ICameraState state);
    void PopState();
    void Update(float deltaTime);

private:
    std::vector<ICameraState> m_stack;
};
