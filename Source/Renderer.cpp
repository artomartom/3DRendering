#pragma once

#include "pch.hpp"
#include "Renderer.hpp"

using ::Microsoft::WRL::ComPtr;

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

        auto pDevice{m_pDeviceResource->GetDevice()};
        D3D11_BUFFER_DESC d_ConstBuffer{};
        d_ConstBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_ConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_ConstBuffer.CPUAccessFlags = 0;

        {
            d_ConstBuffer.ByteWidth = (sizeof(ModelBuffer) + 15) / 16 * 16;
            if (H_FAIL(hr = pDevice->CreateBuffer(&d_ConstBuffer, nullptr, &m_pModelBuffer)))
                return hr;
        };
        {
            d_ConstBuffer.ByteWidth = (sizeof(FrameBuffer) + 15) / 16 * 16;
            if (H_FAIL(hr = pDevice->CreateBuffer(&d_ConstBuffer, nullptr, &m_pFrameBuffer)))
                return hr;
        };
        {
            d_ConstBuffer.ByteWidth = (sizeof(ProjectionBuffer) + 15) / 16 * 16;
            if (H_FAIL(hr = pDevice->CreateBuffer(&d_ConstBuffer, nullptr, &m_pProjBuffer)))
                return hr;
        };
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
        m_pModelBuffer.Get(),
        m_pFrameBuffer.Get(),
        m_pProjBuffer.Get(),
    };

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
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
    using namespace ::DirectX;
    ProjectionBuffer Buffer{};
    m_viewPort.Width = Width;
    m_viewPort.Height = Height;
    m_camera.SetProjParams(XM_PI / 4, Width / Height, 0.01f, 100.0f);
    XMStoreFloat4x4(&Buffer.m_projection, XMMatrixTranspose(m_camera.Projection()));
    m_pContext->UpdateSubresource(m_pProjBuffer.Get(), 0, nullptr, &Buffer, 0, 0);
};
void Renderer::UpdateFrameBuffer() noexcept
{
    using namespace ::DirectX;
    FrameBuffer frameBuffer{};
    // Update Timer
    m_timer.Count();

    frameBuffer.m_time.x = m_timer.GetCount<float>() / 1000.f;
    frameBuffer.m_time.y = m_timer.GetDelta<float>();
    // static float camX{};
    // camX += 1.0f;
    // m_camera.Eye({sin(XMConvertToRadians(camX)), 0.0f, cos(XMConvertToRadians(camX) + 2.0f)});
    XMStoreFloat4x4(&frameBuffer.m_view, XMMatrixTranspose(m_camera.View()));
    m_pContext->UpdateSubresource(m_pFrameBuffer.Get(), 0, nullptr, &frameBuffer, 0, 0);
};
void Renderer::Draw() const noexcept
{
    using namespace ::DirectX;
    ModelBuffer modelBuffer{};
    XMStoreFloat4x4(&modelBuffer.m_model, XMMatrixTranspose(m_cube.Model()));
    m_pContext->UpdateSubresource(m_pModelBuffer.Get(), 0, nullptr, &modelBuffer, 0, 0);

    m_pContext->ClearRenderTargetView(Renderer::m_pRTV.Get(), &m_RTVClearColor.x);
    m_pContext->RSSetViewports(1, &m_viewPort);
    //    m_pContext->OMSetRenderTargets(1u, Renderer::m_pRTV.GetAddressOf(), m_pDepthStencilView.Get()); //something missing
    m_pContext->OMSetRenderTargets(1u, Renderer::m_pRTV.GetAddressOf(), nullptr);
    m_pContext->Draw(36u, 0u);
};