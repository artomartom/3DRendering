
#include "pch.hpp"
#include "Camera.hpp"

#undef min // Use __min instead.
#undef max // Use __max instead.

Camera::Camera()
    : m_eye{-2.0f, 2.0f, 5.0f},
      m_lookAt{0.0f, 0.0f, 1.0f},
      m_up{0.0f, 1.0f, 0.0f},
      m_cameraSpeed{5.0f}
{
    View();
    SetProjParams(::DirectX::XM_PI / 4, 1.0f, 1.0f, 1000.0f);
}

void Camera::LookDirection(_In_ const ::DirectX::XMFLOAT3 &lookDirection)
{
    m_lookAt.x = m_eye.x + lookDirection.x;
    m_lookAt.y = m_eye.y + lookDirection.y;
    m_lookAt.z = m_eye.z + lookDirection.z;
}

void Camera::Eye(_In_ const ::DirectX::XMFLOAT3 &eye)
{
    m_eye = eye;
}

void Camera::MoveF(float deltaT) noexcept
{
    m_eye.z -= deltaT * m_cameraSpeed;
};
void Camera::MoveB(float deltaT) noexcept
{
    m_eye.z += deltaT * m_cameraSpeed;
};
void Camera::MoveL(float deltaT) noexcept
{
    m_eye.x -= deltaT * m_cameraSpeed;
};
void Camera::MoveR(float deltaT) noexcept
{
    m_eye.x += deltaT * m_cameraSpeed;
};

::DirectX::XMMATRIX Camera::View()
{
    using namespace DirectX;
    // Calculate the view matrix.
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&m_eye),
        XMLoadFloat3(&m_lookAt),
        XMLoadFloat3(&m_up));

    XMVECTOR det;
    XMMATRIX inverseView = XMMatrixInverse(&det, view);
    XMStoreFloat4x4(&m_viewMatrix, view);
    XMStoreFloat4x4(&m_inverseView, inverseView);

    // The axis basis vectors and camera position are stored inside the
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector.
    XMFLOAT3 zBasis;
    XMStoreFloat3(&zBasis, inverseView.r[2]);

    m_cameraYawAngle = atan2f(zBasis.x, zBasis.z);

    float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
    m_cameraPitchAngle = atan2f(zBasis.y, len);
    return view;
}

void Camera::SetProjParams(
    _In_ float fieldOfView,
    _In_ float aspectRatio,
    _In_ float nearPlane,
    _In_ float farPlane)
{
    using namespace ::DirectX;
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
