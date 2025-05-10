#pragma once
#include <vector>

#include "ITurnState.hpp"

class WidgetTurnSequence;
struct InitiativeEntry;
class Character;
class TurnBaseSubsystem;

class StateRound : public ITurnState
{
public:
    explicit StateRound();

    void OnInit() override;
    void Update(float dt) override;
    bool GetIsFinished() const override;
    void OnEnter() override;
    void Exit() override;

    /// Getters
    std::vector<InitiativeEntry>& GetInitiativeEntries() { return m_entries; }
    size_t                        GetCharacterRoundIndex() { return m_ix; }

private:
    void BuildInitiativeList(); // roll & sort
    void AdvanceIndex(); // 进入下一行动者 / 下一回合
    int  RollFor(Character* c) const;

    std::vector<InitiativeEntry> m_entries;
    size_t                       m_ix = 0;

    bool m_bBattleEnded = false;
};
