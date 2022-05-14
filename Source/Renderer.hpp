
#ifndef RENDER_HPP
#define RENDER_HPP
#pragma once
#include "DeviceResource.hpp"
#include "FrameBuffer.hpp"

class Renderer
{

    const ::Microsoft::WRL::ComPtr<ID3D11DeviceContext> &GetContext() const { return m_pContext; };
    const ::Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &GetRenderTargetView() const { return m_pRTV; };

protected:
    Renderer() = default;
    HRESULT Initialize();
    void SetPipeLine() const noexcept;
    void UpdateViewPortSize(float Width, float Height) noexcept;
    void UpdateFrameBuffer() noexcept;
    void SetViewPort(float Width, float Height) noexcept;
    void SwitchTopology() noexcept;
    void Draw() const noexcept;

    std::unique_ptr<DeviceResource> m_pDeviceResource{};
    FrameBuffer m_FrameBuffer{};
    D3D11_VIEWPORT m_ViewPort{0.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    DirectX::XMFLOAT4 RTVClearColor{0.0f, 0.0f, 0.0f, 0.99f};

    Time::Timer Timer{};

    ::Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext{};
    ::Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRenderTarget{};
    ::Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV{};
    ::Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil{};
    ::Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView{};

    ::Microsoft::WRL::ComPtr<ID3D11Buffer> m_pFrameBuffer{}; // constant buffer:changes every update

    ::Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout{};
    ::Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader{};
    ::Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader{};
};

#endif // RENDER_HPP