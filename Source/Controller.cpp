

#pragma once
#include "pch.hpp"
#define IMGUI_DISABLE // dont include imgui in this  cpp
#include "Controller.hpp"

using namespace ::DirectX;

Controller::Controller(const RECT &boundingBox)
    : m_boxLeftTop{boundingBox.left, boundingBox.top},
      m_boxRightBottom{boundingBox.right, boundingBox.bottom} {};

XMFLOAT3 Controller::LookDirection()
{
    XMFLOAT3 lookDirection{};
    float r = cosf(m_pitch);           // In the plane.
    lookDirection.y = sinf(m_pitch);   // Vertical.
    lookDirection.z = r * cosf(m_yaw); // Fwd-back.
    lookDirection.x = r * sinf(m_yaw); // Left-right.
    return lookDirection;
}
void Controller::LastPos(POINT lastPos) noexcept
{
    m_lastCursorPos.x = uint32_t(lastPos.x);
    m_lastCursorPos.y = uint32_t(lastPos.y);
};

void Controller::CursorMoved(POINT newPoint)
{
    ::POINT deltaPos{newPoint};

    deltaPos.x -= m_lastCursorPos.x;
    deltaPos.y -= m_lastCursorPos.y;

    m_lastCursorPos.x = newPoint.x;
    m_lastCursorPos.y = newPoint.y;

    m_pitch += static_cast<float>(deltaPos.y) * cursorSpeed;
    m_yaw -= static_cast<float>(deltaPos.x) * cursorSpeed;

    constexpr float limit{XM_PI / 4.0f - 0.01f};
    m_pitch = __max(-limit, m_pitch);
    m_pitch = __min(+limit, m_pitch);

    if (m_yaw > XM_PI)
    {
        m_yaw -= XM_PI * 2.0f;
    }
    else if (m_yaw < -XM_PI)
    {
        m_yaw += XM_PI * 2.0f;
    }
};