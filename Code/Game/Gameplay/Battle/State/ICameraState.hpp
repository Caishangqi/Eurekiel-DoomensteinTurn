#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Game/Framework/PlayerController.hpp"

class CameraDirector;

struct CameraStateMetaData
{
    friend class CameraDirector;
    friend class ICameraState;

    CameraStateMetaData();


    Vec3  m_targetPosition   = Vec3::ZERO;
    Vec3  m_targetRotation   = Vec3::ZERO;
    float m_targetFOV        = 0.0f;
    float m_lengthOfDuration = 1.0f;

    SpriteAnimPlaybackType m_playbackType  = SpriteAnimPlaybackType::LOOP;
    bool                   m_bRestWhenExit = false;

protected:
    Vec3  m_cachedPosition = Vec3::ZERO;
    Vec3  m_cachedRotation = Vec3::ZERO;
    float m_cachedFOV      = 0.0f;
};

class ICameraState
{
public:
    ICameraState(CameraStateMetaData metaData, PlayerController* controller);
    virtual      ~ICameraState() = default;
    virtual void Enter();
    virtual void Exit();
    virtual void Update(float dt);

    bool GetIsFinished() const;

private:
    void CacheCameraInformation(CameraStateMetaData& metaData, PlayerController* controller);

    CameraDirector*     m_ownerDirector = nullptr;
    PlayerController*   m_controller    = nullptr;
    CameraStateMetaData m_metaData;
    Timer*              m_timer       = nullptr;
    bool                m_bIsFinished = false;
};
