
#include "pch.hpp"
#include "Camera.hpp"

#undef min // Use __min instead.
#undef max // Use __max instead.
using namespace DirectX;
Camera::Camera()
    : m_eye{0.0f, 2.0f, -2.0f},
      m_lookAt{0.0f, 0.0f, 1.0f},
      m_up{0.0f, 1.0f, 0.0f},
      m_right{1.0f, 0.0f, 0.0f},
      m_cameraSpeed{5.0f}
{
    View();
    SetProjParams(XM_PI / 4, 1.0f, 1.0f, 1000.0f);
}

void Camera::LookDirection(_In_ const XMFLOAT3 &lookDirection)
{
    m_lookAt.x = m_eye.x + lookDirection.x;
    m_lookAt.y = m_eye.y + lookDirection.y;
    m_lookAt.z = m_eye.z + lookDirection.z;
    XMStoreFloat3(&m_right, XMVector3Cross(XMLoadFloat3(&m_lookAt), XMLoadFloat3(&m_up)));
};

void Camera::Eye(_In_ const XMFLOAT3 &eye)
{
    m_eye = eye;
}

void Camera::MoveF(float deltaT) noexcept
{
    m_eye.x += m_lookAt.x * (deltaT * m_cameraSpeed);
    m_eye.y += m_lookAt.y * (deltaT * m_cameraSpeed);
    m_eye.z += m_lookAt.z * (deltaT * m_cameraSpeed);
};
void Camera::MoveB(float deltaT) noexcept
{
    m_eye.x -= m_lookAt.x * (deltaT * m_cameraSpeed);
    m_eye.y -= m_lookAt.y * (deltaT * m_cameraSpeed);
    m_eye.z -= m_lookAt.z * (deltaT * m_cameraSpeed);
};
void Camera::MoveL(float deltaT) noexcept
{
    m_eye.x += m_right.x * (deltaT * m_cameraSpeed);
    m_eye.y += m_right.y * (deltaT * m_cameraSpeed);
    m_eye.z += m_right.z * (deltaT * m_cameraSpeed);
};
void Camera::MoveR(float deltaT) noexcept
{

    m_eye.x -= m_right.x * (deltaT * m_cameraSpeed);
    m_eye.y -= m_right.y * (deltaT * m_cameraSpeed);
    m_eye.z -= m_right.z * (deltaT * m_cameraSpeed);
};

XMMATRIX Camera::View()
{

    // Calculate the view matrix.
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&m_eye),
        XMLoadFloat3(&m_lookAt),
        XMLoadFloat3(&m_up));
    return view;
}

void Camera::SetProjParams(
    _In_ float fieldOfView,
    _In_ float aspectRatio,
    _In_ float nearPlane,
    _In_ float farPlane)
{
    // Set attributes for the projection matrix.
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    XMStoreFloat4x4(
        &m_projectionMatrix,
        XMMatrixPerspectiveFovLH(
            m_fieldOfView,
            m_aspectRatio,
            m_nearPlane,
            m_farPlane));
}
