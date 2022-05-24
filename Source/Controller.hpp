#pragma once

class Controller
{
public:
    ::DirectX::XMFLOAT3 LookDirection()
    {
        ::DirectX::XMFLOAT3 lookDirection{};

        float r = cosf(m_pitch);           // In the plane.
        lookDirection.y = sinf(m_pitch);   // Vertical.
        lookDirection.z = r * cosf(m_yaw); // Fwd-back.
        lookDirection.x = r * sinf(m_yaw); // Left-right.
        return lookDirection;
    }
    void CursorMoved(POINT newPoint)
    {
        using namespace ::DirectX;
        POINT deltaPos{newPoint.x, newPoint.y};

        deltaPos.x -= m_lastCursorPos.x;
        deltaPos.y -= m_lastCursorPos.y;

        m_lastCursorPos.x = newPoint.x;
        m_lastCursorPos.y = newPoint.y;

        m_pitch -= static_cast<float>(deltaPos.y) * cursorSpeed;
        m_yaw += static_cast<float>(deltaPos.x) * cursorSpeed;

        float limit = XM_PI / 2.0f - 0.01f;
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
        ::DirectX::XMUINT2 LastPos()const noexcept{return m_lastCursorPos;};


    // args.pos.x / m_viewPort.Width, 0.f, args.pos.y / m_viewPort.Height
private:
    ::DirectX::XMUINT2 m_lastCursorPos{};
    float m_pitch;
    float m_yaw;
    static constexpr float cursorSpeed{.0001f};
};