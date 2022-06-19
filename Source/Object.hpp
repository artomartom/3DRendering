#pragma once

class Object
{
public:
    Object()
        : m_position{::DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)}
    {
        using namespace ::DirectX;
        XMStoreFloat4x4(&m_model, XMMatrixIdentity());
    };

    ::DirectX::XMMATRIX Model() const noexcept { return ::DirectX::XMLoadFloat4x4(&m_model); };

protected:
private:
    ::DirectX::XMFLOAT4X4 m_model;
    ::DirectX::XMFLOAT3 m_position;
};
