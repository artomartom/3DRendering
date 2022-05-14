
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

/**
 * represents constant buffer layout in vertex shader, which stors wvp matrix and time variable.
 * updated every frame
 */
class FrameBuffer
{
public:
    FrameBuffer()
    {
        SetView();
        SetWorld(1.0f);
    };
    ::DirectX::XMFLOAT2 GetTime() const noexcept { return time; };

    void SetTime(long long st, float deltaT) noexcept
    {

        time.x = static_cast<float>(st) / 1000.f;
        time.y = deltaT;
    };
    void SetWorld(float angle) noexcept
    {
        using namespace ::DirectX;
        World = XMMatrixTranspose(XMMatrixRotationY(angle));
    };
    void SetView() noexcept
    {
        using namespace ::DirectX;
        XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
        XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        View = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
    };
    void SetProjection(D3D11_VIEWPORT &ViewPort) noexcept
    {
        using namespace ::DirectX;
        Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, ViewPort.Width / ViewPort.Height, 0.1f, 100.f));
    };

private:
    ::DirectX::XMFLOAT2 time{}; // {secFromStart.milisec, deltaTime}
    ::DirectX::XMMATRIX World{};
    ::DirectX::XMMATRIX View{};
    ::DirectX::XMMATRIX Projection{};
};
#endif // FRAMEBUFFER_HPP