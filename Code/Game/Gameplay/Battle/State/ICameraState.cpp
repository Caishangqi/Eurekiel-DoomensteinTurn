#include "ICameraState.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Game.hpp"


CameraStateMetaData::CameraStateMetaData()
{
}

ICameraState::ICameraState(CameraStateMetaData metaData, PlayerController* controller): m_controller(controller), m_metaData(metaData)
{
    CacheCameraInformation(m_metaData, m_controller);
    m_timer = new Timer(metaData.m_lengthOfDuration, g_theGame->m_clock);
}

void ICameraState::Enter()
{
    m_timer->Start();
}

void ICameraState::Exit()
{
    m_timer->Stop();
    delete m_timer;
    m_timer = nullptr;

    if (m_metaData.m_bRestWhenExit)
    {
        m_controller->m_position                      = m_metaData.m_cachedPosition;
        m_controller->m_orientation                   = EulerAngles(m_controller->m_orientation);
        m_controller->m_worldCamera->m_perspectiveFOV = m_metaData.m_cachedFOV;
    }
}

void ICameraState::Update(float dt)
{
    if (m_bIsFinished) return;
    float delta = dt * m_metaData.m_lengthOfDuration;
    UNUSED(delta)
    float fraction = 0.f;
    if (m_metaData.m_playbackType == SpriteAnimPlaybackType::PING_PONG)
    {
        fraction = 0.5f * (sinf(2.0f * M_PI * m_timer->GetElapsedTime() / m_metaData.m_lengthOfDuration) + 1.0f);
    }
    else if (m_metaData.m_playbackType == SpriteAnimPlaybackType::ONCE)
    {
        fraction = m_timer->GetElapsedFraction();
        if (fraction >= 1.f)
        {
            m_bIsFinished = true;
        }
    }

    Vec3  interpolatePos = Interpolate(m_metaData.m_cachedPosition, m_metaData.m_targetPosition, fraction);
    Vec3  interpolateRot = Interpolate(m_metaData.m_cachedRotation, m_metaData.m_targetRotation, fraction);
    float interpolateFov = Interpolate(m_metaData.m_cachedFOV, m_metaData.m_targetFOV, fraction);

    /// Update
    m_controller->m_position                      = interpolatePos;
    m_controller->m_orientation                   = EulerAngles(interpolateRot);
    m_controller->m_worldCamera->m_perspectiveFOV = interpolateFov;
}

bool ICameraState::GetIsFinished() const
{
    return m_bIsFinished;
}

void ICameraState::CacheCameraInformation(CameraStateMetaData& metaData, PlayerController* controller)
{
    metaData.m_cachedFOV      = controller->m_worldCamera->m_perspectiveFOV;
    metaData.m_cachedPosition = controller->m_position;
    metaData.m_cachedRotation = Vec3(controller->m_orientation);
}
