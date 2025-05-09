#pragma once
#include "Game/Framework/Widget.hpp"

class Character;
class Texture;
class StateCharacterTurn;
class StateRound;

class WidgetTurnSequence : public Widget
{
public:
    WidgetTurnSequence();
    ~WidgetTurnSequence() override;

public:
    void Draw() const override;
    void Update() override;

    void InjectRoundState(StateRound* InStateRound);
    void InjectCharacterTurnState(StateCharacterTurn* InStateCharacterTurn);

private:
    void SetRenderState() const;

private:
    StateRound*             m_StateRound         = nullptr;
    StateCharacterTurn*     m_StateCharacterTurn = nullptr;
    std::vector<Character*> m_charactersDrawingSub;

    /// Resource
    Texture* m_sequenceLabel    = nullptr;
    Texture* m_sequenceLabelSub = nullptr;
    /// Geometry
    AABB2              m_boundingBox;
    AABB2              mainSequenceBounding;
    std::vector<AABB2> m_subSequenceBox;
};
