

#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera
{
public:
    using XMMATRIX = ::DirectX::XMMATRIX;
    using XMVECTOR = ::DirectX::XMVECTOR;
    Camera()
        : m_Eye{::DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f)},
          m_At{::DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)},
          m_Up{::DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)},
          m_Right{::DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)} {};
    ::DirectX::XMMATRIX GetWorld()
    {
        using namespace ::DirectX;
        return XMMatrixInverse(nullptr, m_View);
    };
    ::DirectX::XMMATRIX GetView()
    {
        using namespace ::DirectX;
        return XMMatrixLookAtLH(m_Eye, m_At, m_Up);
    };
    void MoveCameraF(float deltaTime) noexcept
    {
        using namespace ::DirectX; // for operator*
        m_Eye += m_At * (m_MovementSpeed * deltaTime);
    };
    void MoveCameraL(float deltaTime) noexcept
    {
        using namespace ::DirectX; // for operator*
        m_Eye -= m_Right * (m_MovementSpeed * deltaTime);
    };
    void MoveCameraB(float deltaTime) noexcept
    {
        using namespace ::DirectX; // for operator*
        m_Eye -= m_At * (m_MovementSpeed * deltaTime);
    };
    void MoveCameraR(float deltaTime) noexcept
    {
        using namespace ::DirectX; // for operator*
        m_Eye += m_Right * (m_MovementSpeed * deltaTime);
    };
    void Update() noexcept
    {
        //    using namespace ::DirectX;
        //    // calculate the new Front vector
        //    XMVECTOR front{
        //        cos(m_Yaw) * cos(m_Pitch),
        //        sin(m_Pitch),
        //        sin(m_Yaw) * cos(m_Pitch)};
        //    m_At = XMVector3Normalize(front);
        //    // also re-calculate the Right and Up vector
        //    m_Right = XMVector3Normalize(XMVector3Cross(m_At, m_WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        //    m_Up = XMVector3Normalize(XMVector3Cross(m_Right, m_At));
    }

private:
    XMVECTOR m_Eye{};
    XMVECTOR m_At{};
    XMVECTOR m_Up{};
    XMVECTOR m_Right{};
    // XMVECTOR m_WorldUp{m_Up};

    // euler Angles
    // float m_Yaw{::DirectX::XMConvertToRadians(-90.0f)};
    // float m_Pitch{::DirectX::XMConvertToRadians(0.0f)};
    // camera options
    float m_MovementSpeed{10.f};
    // float m_MouseSensitivity{0.1f};
    // float m_Zoom{45.0f};
};
#endif // CAMERA_HPP