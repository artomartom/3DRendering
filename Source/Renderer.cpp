#pragma once

#include "pch.hpp"
#include "Renderer.hpp"

using ::Microsoft::WRL::ComPtr;
using namespace ::DirectX;

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
     *     Create Constant Buffer
     */
    {
        D3D11_BUFFER_DESC d_ConstBuffer{};
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

void Renderer::UpdateViewPortSize(float Width, float Height) noexcept
{
    m_ViewPort.Width = Width;
    m_ViewPort.Height = Height;
    m_FrameBuffer.SetProjection(m_ViewPort);
};

void Renderer::UpdateFrameBuffer() noexcept
{
    // Update Timer
    m_FrameBuffer.SetTime(Timer.Count<long>(), Timer.GetDelta<float>());
    m_FrameBuffer.SetWorld(m_FrameBuffer.GetTime().x);
    m_pContext->UpdateSubresource(m_pFrameBuffer.Get(), 0, nullptr, &m_FrameBuffer, 0, 0);
};

void Renderer::Draw() const noexcept
{

    m_pContext->ClearRenderTargetView(Renderer::m_pRTV.Get(), &RTVClearColor.x);
    m_pContext->RSSetViewports(1, &m_ViewPort);
    m_pContext->OMSetRenderTargets(1u, Renderer::m_pRTV.GetAddressOf(), m_pDepthStencilView.Get());
    m_pContext->Draw(36u, 0u);
};