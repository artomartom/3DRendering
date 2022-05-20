#pragma once

#include "pch.hpp"
#include "Renderer.hpp"
#include "DeviceResource.hpp"

#define CASE(message, action) \
  case message:               \
    action;                   \
    break

using ::Microsoft::WRL::ComPtr;

class App : public CoreApp, public Renderer
{

public:
  static int AppEntry(HINSTANCE hinst)
  {
    DBG_ONLY(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF));

    peekRun(Window::CoreWindow<App>{hinst, {50, 50, 1700, 1000}});
    MessageBeep(5);
    return 0;
  };

  void OnWindowActivate(_In_ const ::Window::ActivateArgs &args) noexcept
  {

    if (CoreApp::m_IsVisible != args.IsMinimized)
    {
      CoreApp::m_IsVisible = args.IsMinimized;
      m_ShouldDraw = !CoreApp::m_IsVisible;
    };
  };

  void OnKeyStroke(_In_ const ::Window::KeyEventArgs &args) noexcept
  {
    switch (args.VirtualKey)
    {
      CASE(VK_ESCAPE, { CoreApp::Close(); });
      CASE(VK_SPACE, { Renderer::Timer.Switch(); });
      CASE(VK_TAB, { Renderer::SwitchTopology(); });
      CASE('W', { Renderer::m_Camera.MoveCameraF(Renderer::Timer.GetDelta<float>()); });
      CASE('A', { Renderer::m_Camera.MoveCameraL(Renderer::Timer.GetDelta<float>()); });
      CASE('S', { Renderer::m_Camera.MoveCameraB(Renderer::Timer.GetDelta<float>()); });
      CASE('D', { Renderer::m_Camera.MoveCameraR(Renderer::Timer.GetDelta<float>()); });
    }
  };
  void OnCreate(_In_ const ::Window::CreationArgs &args) noexcept
  {
    HRESULT hr{};

    SIZE RTSize{RECTWIDTH(args.Rect), RECTHEIGHT(args.Rect)};
    m_pDeviceResource = std::make_unique<DeviceResource>(&m_pContext, &hr);
    if (H_OK(hr))
    {
      // CreateSizeDependentDeviceResources is not called from here because of OnSizeChanged message being send automatically after OnCreate's success
      if (H_OK(hr = Renderer::Initialize()))
      {
        Renderer::SetPipeLine();
        return;
      };
    };
    DBG_ONLY(
        {
          Log<File>::Write(L"Pulling Debug Messages before closing...");
          m_pDeviceResource->PullDebugMessage();
        });
    m_ShouldClose = true;
  };

  void OnSizeChanged(_In_ const ::Window::SizeChangedArgs &args) noexcept
  {
    float NewWidth{static_cast<float>(args.New.cx)};
    float NewHeight{static_cast<float>(args.New.cy)};

    switch (args.Type)
    {
      CASE(::Window::SizeChangedArgs::Type::Minimized, { m_pDeviceResource->GetSwapChain()->SetFullscreenState(false, nullptr); });
      CASE(::Window::SizeChangedArgs::Type::Maximized, { m_pDeviceResource->GetSwapChain()->SetFullscreenState(true, nullptr); });
    default:
      break;
    }
    if (m_ViewPort.Width == NewWidth && m_ViewPort.Height == NewHeight)
    {
      return;
    }
    else
    {
      Renderer::UpdateViewPortSize(NewWidth, NewHeight);
     H_FAIL( m_pDeviceResource->CreateSizeDependentDeviceResources(m_Handle, m_ViewPort, m_pContext.Get(),
                                                            &m_pRenderTarget, &m_pRTV,
                                                            &m_pDepthStencil, &m_pDepthStencilView));
    }
  };

  void Draw() noexcept
  {

    if (m_ShouldDraw && !m_ShouldClose)
    {
      Renderer::UpdateFrameBuffer();
      Renderer::Draw();
      /**
       *   The first argument instructs DXGI to block until VSync, putting the application
       *  to sleep until the next VSync. This ensures we don't waste any cycles rendering
       *  frames that will never be displayed to the screen.
       */
      H_FAIL(m_pDeviceResource->Present());
    };
    DBG_ONLY(m_pDeviceResource->DebugInterface::PullDebugMessage());
  };

  void OnClose() noexcept
  {
    /**
     * You may not release a swap chain in full-screen mode
     * because doing so may create thread contention (which
     * will cause DXGI to raise a non-continuable exception).
     */
    m_pDeviceResource->GetSwapChain()->SetFullscreenState(false, nullptr);
  };

private:
  bool m_ShouldDraw{true};
  bool m_ShouldClose{false};

  template <class TWindow>
  friend int __stdcall peekRun(TWindow &&window)
  {
    if (window.m_ShouldClose)
    {
      Log<File>::Write(L"App Creation canceled");
      return 1;
    };
    ::MSG messages{};
    while (messages.message != WM_QUIT)
    {
      ::PeekMessageW(&messages, 0, 0, 0, PM_REMOVE);
      ::TranslateMessage(&messages);
      ::DispatchMessageW(&messages);
      if (window.m_ShouldDraw)
        window.App::Draw();
    };
    return 0;
  };
};

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
int wWinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) { return Invoke(&App::AppEntry, hinst); };
int main() { return Invoke(&App::AppEntry, (HINSTANCE)&__ImageBase); };