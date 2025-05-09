#pragma once
#include "Engine/Core/Timer.hpp"
#include "Game/Framework/Widget.hpp"

class WidgetDamageIndicator : public Widget
{
public:
    WidgetDamageIndicator();
    ~WidgetDamageIndicator() override;

    void OnInit() override;
    void Draw() const override;
    void Update() override;

    Vec3        m_position     = Vec3::ZERO;
    float       m_duration     = 0.5f;
    std::string m_data         = "";
    Rgba8       m_startColor   = Rgba8::WHITE;
    Rgba8       m_endColor     = Rgba8::WHITE;
    bool        m_bInterpolate = false;
    Vec2        m_dimensions   = Vec2::ZERO;
    float       m_size         = 50.f;

private:
    Timer* m_timer            = nullptr;
    Rgba8  m_interpolateColor = Rgba8::WHITE;
    Vec2   m_screenPos        = Vec2::ZERO;

    BitmapFont* g_testFont = nullptr;
};
