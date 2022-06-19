#pragma once

class Controller
{
public:
    Controller() = default;
    Controller(const RECT &boundingBox);

    void CursorMoved(POINT newPoint);
    void LastPos(::DirectX::XMUINT2 lastPos) noexcept { m_lastCursorPos = lastPos; };
    void LastPos(POINT lastPos) noexcept;

    ::DirectX::XMFLOAT3 LookDirection();

    float Pitch() const noexcept { return m_pitch; };
    float Yaw() const noexcept { return m_yaw; };
    ::DirectX::XMUINT2 LastPos() const noexcept { return m_lastCursorPos; };

#ifndef IMGUI_DISABLE
    void ImGuiWindow()
    {
        using namespace DirectX;
        ImGui::Text("Controller:Pitch %.1f", &m_pitch);
        ImGui::Text("Controller:Yaw %.1f", &m_yaw);
        ImGui::Text("Controller:lastpos %i, %i", m_lastCursorPos.x, m_lastCursorPos.y);
    }

#endif // IMGUI_DISABLE

    // args.pos.x / m_viewPort.Width, 0.f, args.pos.y / m_viewPort.Height
private:
    ::DirectX::XMUINT2 m_lastCursorPos{};
    POINT m_boxLeftTop{};
    POINT m_boxRightBottom{};
    float m_pitch{};
    float m_yaw{};
    static constexpr float cursorSpeed{.001f};
};