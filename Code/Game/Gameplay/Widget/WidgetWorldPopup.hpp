#pragma once
#include "Game/Framework/Widget.hpp"

class Texture;

class WidgetWorldPopup : public Widget
{
public:
    WidgetWorldPopup(Vec3& position);
    WidgetWorldPopup(Vec3& position, Actor* pointActor);
    ~WidgetWorldPopup() override;

public:
    void Draw() const override;
    void Update() override;

    /// Setter
    void SetWorldPosition(Vec3& position) { m_worldPosition = position; }
    void SetPointActor(Actor* pointActor) { m_pointActor = pointActor; }

private:
    Vec3     m_worldPosition         = Vec3::ZERO;
    Actor*   m_pointActor            = nullptr;
    Texture* m_textureInteractButton = nullptr;
    Texture* m_textureInteractIcon   = nullptr;
};
