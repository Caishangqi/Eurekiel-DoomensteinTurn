#include "StateRound.hpp"

#include <algorithm>

#include "StateCharacterTurn.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/Scene.hpp"
#include "Game/Gameplay/Widget/WidgetTurnSequence.hpp"

StateRound::StateRound()
{
    m_turnStateName = "StateRound";
}

void StateRound::OnInit()
{
    BuildInitiativeList(); // Speed Sequence
    m_turnBaseSubsystem->GetWidgetTurnSequence()->InjectRoundState(this);
    for (auto& e : m_entries)
        e.character->OnRoundStart();
}

void StateRound::Update(float dt)
{
    // 1) 让顶层状态（StateCharacterTurn）跑逻辑
    /*
    if (!m_turnBaseSubsystem->GetStateStackEmptyExpectRound())
        m_turnBaseSubsystem->GetStateStackBack()->Update(dt);
        */

    // 2) 顶层状态结束了？（角色行动完）
    if (m_turnBaseSubsystem->GetStateStackEmptyExpectRound() && m_turnBaseSubsystem->GetPendingPushEmpty())
    {
        // 2-b) 轮到下一个角色
        AdvanceIndex();
        // 2-c) 如果这一回合还有角色，就 Push 下一个 StateCharacterTurn
        if (m_ix < m_entries.size())
        {
            Character* next = m_turnBaseSubsystem->GetCurrentScene()->GetCharacterByHandle(m_entries[m_ix].character->GetHandle());
            if (next)
                m_turnBaseSubsystem->PushState(new StateCharacterTurn(next));
        }
        else
        {
            // 所有人都行动完 —— StateRound 可以收尾退出
            m_isFinished = true;
        }
    }
}

bool StateRound::GetIsFinished() const
{
    return m_isFinished;
}

void StateRound::OnEnter()
{
    ITurnState::OnEnter();
    if (!m_entries.empty())
        m_turnBaseSubsystem->PushState(
            new StateCharacterTurn(m_entries[m_ix].character));
}

void StateRound::Exit()
{
    for (auto& e : m_entries)
        e.character->OnRoundEnd(); // Buff 回合递减等

    m_entries.clear();
    m_ix = 0;
}

void StateRound::BuildInitiativeList()
{
    // 1. reset
    m_entries.clear();
    m_ix = 0;

    // 2. 收集可行动单位
    const auto& chars = m_turnBaseSubsystem->GetCurrentScene()->GetCharacters();
    for (Character* c : chars)
    {
        if (!c || c->GetIsDead()) continue; // 死亡→跳过
        if (!c->CanAct()) continue; // 被永久晕 / 召唤未就绪……

        // 3. 计算 score
        int characterInitiative = c->RollInitiative();
        int tiebreak            = (c->GetUniqueID() & 0xF); // 低 4 位打散同速

        InitiativeEntry e;
        e.character = c;
        e.score     = characterInitiative + tiebreak;
        m_entries.push_back(e);
    }

    // 4. 排序（大 → 小）
    std::sort(m_entries.begin(), m_entries.end(),
              [](const InitiativeEntry& a, const InitiativeEntry& b)
              {
                  return a.score > b.score;
              });
}

void StateRound::AdvanceIndex()
{
    if (++m_ix >= m_entries.size())
    {
        /*  回合结束：处理 buff / DOT / HOT  */
        for (auto& e : m_entries)
        {
            if (m_turnBaseSubsystem->GetCurrentScene()->GetCharacterByHandle(e.character->GetHandle()))
                e.character->OnRoundEnd();
        }
        BuildInitiativeList(); // 重新 roll，得到新顺序
        m_ix = 0;
    }
}

int StateRound::RollFor(Character* c) const
{
    return c->RollInitiative();
}
