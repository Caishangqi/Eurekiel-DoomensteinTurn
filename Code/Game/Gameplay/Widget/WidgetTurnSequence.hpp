#pragma once
#include "Game/Framework/Widget.hpp"

class WidgetTurnSequence : public Widget
{
public:
    WidgetTurnSequence();
    ~WidgetTurnSequence() override;

public:
    void Draw() const override;
    void Update() override;
};
