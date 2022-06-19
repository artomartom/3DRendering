

#ifndef CAMERA_HPP
#define CAMERA_HPP

#ifndef IMGUI_DISABLE
#include "ImGui//ImGuiMngr.hpp"
#endif // IMGUI_DISABLE

class Camera
{
public:
    Camera();
    Camera(const Camera &) = delete;
    void operator=(const Camera &) = delete;
    Camera(Camera &&) = delete;
    void operator=(Camera &&) = delete;

    void SetProjParams(_In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane);

    void LookDirection(_In_ const ::DirectX::XMFLOAT3 &lookDirection);
    void Eye(_In_ const ::DirectX::XMFLOAT3 &position);

    void MoveF(float deltaT) noexcept;
    void MoveL(float deltaT) noexcept;
    void MoveB(float deltaT) noexcept;
    void MoveR(float deltaT) noexcept;

    ::DirectX::XMMATRIX View();
    ::DirectX::XMMATRIX Projection() { return ::DirectX::XMLoadFloat4x4(&m_projectionMatrix); };
    ::DirectX::XMFLOAT3 Eye() { return m_eye; };
    ::DirectX::XMFLOAT3 LookAt() { return m_lookAt; };
    float NearClipPlane() { return m_nearPlane; };
    float FarClipPlane() { return m_farPlane; };
        // float Pitch() { return m_cameraPitchAngle; };
        // float Yaw() { return m_cameraYawAngle; };
#ifndef IMGUI_DISABLE
    void ImGuiWindow()
    {
        using namespace DirectX;
        ImGui::Text("Camera:Eye %.1f", &m_eye.x);
        ImGui::Text("Camera:LookAt %.1f", &m_lookAt.x);
        ImGui::Text("Camera:Up %.1f", &m_up.x);
        // ImGui::SliderFloat("Yaw", &m_cameraYawAngle, -XM_PI, XM_PI);
        // ImGui::SliderFloat("Pitch", &m_cameraPitchAngle, -XM_PI, XM_PI);
    }

#endif // IMGUI_DISABLE

private:
    ::DirectX::XMFLOAT4X4 m_viewMatrix{};
    ::DirectX::XMFLOAT4X4 m_projectionMatrix{};
    ::DirectX::XMFLOAT4X4 m_inverseView{};
    ::DirectX::XMFLOAT3 m_eye{};
    ::DirectX::XMFLOAT3 m_lookAt{};
    ::DirectX::XMFLOAT3 m_up{};
    ::DirectX::XMFLOAT3 m_right{};
    float m_cameraSpeed{};
    // float m_cameraYawAngle{};
    // float m_cameraPitchAngle{};
    float m_fieldOfView{};
    float m_aspectRatio{};
    float m_nearPlane{};
    float m_farPlane{};
};

#endif // CAMERA_HPP