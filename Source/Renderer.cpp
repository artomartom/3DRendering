#pragma once

#include "pch.hpp"
#include "Renderer.hpp"

using ::Microsoft::WRL::ComPtr;
using namespace ::DirectX;

struct ViewPortSizeBuffer
{
    explicit ViewPortSizeBuffer(float x, float y)
        : ViewPortSize{x, y}, aspectRatio{(x <= y) ? 1.0 : y / x, (x > y) ? 1.0f : x / y} {};
    DirectX::XMFLOAT2 ViewPortSize{};
    DirectX::XMFLOAT2 aspectRatio{};
};

struct FrameBuffer
{
    static XMMATRIX GetWorld(float angle) { return XMMatrixTranspose(XMMatrixRotationY(angle)); };
    static XMMATRIX GetView()
    {
        XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
        XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        return XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
    };
    static XMMATRIX GetProjection(D3D11_VIEWPORT &&ViewPort)
    {
        return XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, ViewPort.Width / ViewPort.Height, 0.1f, 100.f));
    };

    explicit FrameBuffer(long long st, float deltaT, D3D11_VIEWPORT &&ViewPort)
        : t{static_cast<float>(st) / 1000.f, /// sec.milices
            deltaT},
          World{GetWorld(t.x)},
          View{GetView()},
          Projection{GetProjection(std::forward<D3D11_VIEWPORT>(ViewPort))} {/* Log<Console>::Write(t.x, t.y, t.z);*/};
    XMFLOAT2 t{};
    XMMATRIX World{};
    XMMATRIX View{};
    XMMATRIX Projection{};
};

HRESULT Renderer::Initialize()
{
    HRESULT hr{};
    const static std::array<D3D11_INPUT_ELEMENT_DESC, 1> LayoutDescs{
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    if (H_FAIL(hr = m_pDeviceResource->CreateVertexShaderFromFile(L"Vertex.so", LayoutDescs.data(), LayoutDescs.size(), &m_pVertexShader, &m_pInputLayout)))
        return hr;
    if (H_FAIL(hr = m_pDeviceResource->CreatePixelShaderFromFile(L"Pixel.so", &m_pPixelShader)))
        return hr;
    /**
     *     Create Constant Buffers
     */
    {

        // ViewPortSizeBuffer   /*    */ m_pViewPortSizeBuffer
        // FrameBuffer   /*           */ m_pFrameBuffer

        D3D11_BUFFER_DESC d_ConstBuffer{};

        d_ConstBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_ConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_ConstBuffer.CPUAccessFlags = 0;
        d_ConstBuffer.ByteWidth = (sizeof(ViewPortSizeBuffer) + 15) / 16 * 16;
        if (H_FAIL(hr = m_pDeviceResource->GetDevice()->CreateBuffer(&d_ConstBuffer, nullptr, &m_pViewPortSizeBuffer)))
            return hr;

        d_ConstBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_ConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_ConstBuffer.CPUAccessFlags = 0;
        d_ConstBuffer.ByteWidth = (sizeof(FrameBuffer) + 15) / 16 * 16;
        if (H_FAIL(hr = m_pDeviceResource->GetDevice()->CreateBuffer(&d_ConstBuffer, nullptr, &m_pFrameBuffer)))
            return hr;
    };

    return S_OK;
};

void Renderer::SetPipeLine() const noexcept
{

    ID3D11Buffer *buffers[]{nullptr,  // no vertex buffer
                            nullptr}; // no instance buffer
    UINT strides[]{0u, 0u};
    UINT offsets[]{0u, 0u};

    ID3D11Buffer *constBuffers[]{
        m_pViewPortSizeBuffer.Get(),
        m_pFrameBuffer.Get(),
    };

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pContext->IASetInputLayout(m_pInputLayout.Get());
    m_pContext->IASetVertexBuffers(0u, 1u, buffers, strides, offsets);
    // m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
    m_pContext->VSSetConstantBuffers(0, _countof(constBuffers), constBuffers);
    m_pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);

    m_pContext->PSSetConstantBuffers(0, _countof(constBuffers), constBuffers);
    m_pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
};

void Renderer::SwitchTopology() noexcept
{
    static UINT currentIndex{};
    static D3D11_PRIMITIVE_TOPOLOGY s_TopologyList[]{
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
        D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
    };
    currentIndex = (1 + currentIndex) % _countof(s_TopologyList);
    m_pContext->IASetPrimitiveTopology(s_TopologyList[currentIndex]);
};

void Renderer::UpdateViewPortSizeBuffer(float Width, float Height) noexcept
{
    m_ViewPort.Width = Width;
    m_ViewPort.Height = Height;
    ViewPortSizeBuffer ViewPortSize{Width, Height};
    m_pContext->UpdateSubresource(
        m_pViewPortSizeBuffer.Get(),
        0, nullptr, &ViewPortSize, 0, 0);
};

void Renderer::UpdateFrameBuffer() noexcept
{

    // Update Timer
    FrameBuffer constantBuffer(Timer.Count<long>(), Timer.GetDelta<float>(), std::forward<D3D11_VIEWPORT>(m_ViewPort));
    m_pContext->UpdateSubresource(
        m_pFrameBuffer.Get(),
        0, nullptr, &constantBuffer, 0, 0);
};

void Renderer::Draw() const noexcept
{

    m_pContext->ClearRenderTargetView(Renderer::m_pRTV.Get(), &RTVClearColor.x);
    m_pContext->RSSetViewports(1, &m_ViewPort);
    m_pContext->OMSetRenderTargets(1u, Renderer::m_pRTV.GetAddressOf(), nullptr);
    m_pContext->Draw(36u, 0u);
};