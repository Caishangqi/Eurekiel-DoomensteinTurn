#pragma once
#include <string>

class TurnBaseSubsystem;

class ITurnState
{
    friend class TurnBaseSubsystem;

public:
    virtual ~ITurnState()
    {
    }

    virtual bool SetContext(TurnBaseSubsystem* sys);

    virtual void OnInit() = 0; // Call When push in the stack
    virtual void OnEnter() = 0; // Call When the first time Turn is updated.
    virtual void Update(float dt) = 0;
    virtual void Exit() = 0;
    virtual bool GetIsFinished() const;
    virtual bool GetIsInit() const;
    virtual bool SetIsInit(bool value);
    virtual bool SetIsFinished(bool value);

protected:
    TurnBaseSubsystem* m_turnBaseSubsystem = nullptr;
    std::string        m_turnStateName     = "Default";
    bool               m_isFinished        = false;
    bool               m_bIsInit           = false;
};
