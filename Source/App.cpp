#pragma once

#include "pch.hpp"
#include "Renderer.hpp"
#include "DeviceResource.hpp"
#include "Controller.hpp"

#define DXMCOMPAT
#include "dxmCompat.hpp"

#define CASE(message, action) \
  case message:               \
    action;                   \
    break

class App : public CoreApp, public Renderer, public Controller
{

public:
  static int AppEntry(HINSTANCE hinst)
  {
    DBG_ONLY(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF));

    PeekRun(Window::CoreWindow<App>{hinst, {50, 50, 1700, 1000}});
    MessageBeep(5);
    return 0;
  };

  void OnWindowActivate(_In_ const ::Window::ActivateArgs &args) noexcept
  {

    if (CoreApp::m_IsVisible != args.IsMinimized)
    {
      CoreApp::m_IsVisible = args.IsMinimized;
      m_shouldDraw = !CoreApp::m_IsVisible;
    };
  };
  void OnCursorMove(_In_ const ::Window::CursorArgs &args) noexcept
  {
    Controller::CursorMoved(args.pos);
    m_camera.LookDirection(Controller::LookDirection());
  };

  void OnKeyStroke(_In_ const ::Window::KeyEventArgs &args) noexcept
  {
    switch (args.VirtualKey)
    {
      CASE(VK_ESCAPE, { CoreApp::Close(); });
      CASE(VK_SPACE, { Renderer::m_timer.Switch(); });
      CASE(VK_TAB, { Renderer::SwitchTopology(); });
      CASE('W', { Renderer::m_camera.MoveF(Renderer::m_timer.GetDelta<float>()); });
      CASE('A', { Renderer::m_camera.MoveL(Renderer::m_timer.GetDelta<float>()); });
      CASE('S', { Renderer::m_camera.MoveB(Renderer::m_timer.GetDelta<float>()); });
      CASE('D', { Renderer::m_camera.MoveR(Renderer::m_timer.GetDelta<float>()); });
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
          // Log<File>::Write(L"Pulling Debug Messages before closing...");
          m_pDeviceResource->PullDebugMessage();
        });
    m_shouldClose = true;
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
    if (m_viewPort.Width == NewWidth && m_viewPort.Height == NewHeight)
    {
      return;
    }
    else
    {
      Renderer::UpdateViewPortSize(NewWidth, NewHeight);
      H_FAIL(m_pDeviceResource->CreateSizeDependentDeviceResources(m_Handle, m_viewPort, m_pContext.Get(),
                                                                   &m_pRenderTarget, &m_pRTV,
                                                                   &m_pDepthStencil, &m_pDepthStencilView));
    }
  };

  void Draw() noexcept
  {
    auto imGuiRender{
        []()
        {
          static float f = 0.0f;
          static int counter = 0;
          static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
          // ImGui::StyleColorsClassic();
          ImGui_ImplDX11_NewFrame();
          ImGui_ImplWin32_NewFrame();
          ImGui::NewFrame();

          ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

          ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

          ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
          ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

          if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
          ImGui::SameLine();
          ImGui::Text("counter = %d", counter);

          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
          ImGui::End();
          ImGui::Render();
          ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }};

    if (m_shouldDraw && !m_shouldClose)
    {
      Renderer::UpdateFrameBuffer();
      Renderer::Draw();

      /**
       *   The first argument instructs DXGI to block until VSync, putting the application
       *  to sleep until the next VSync. This ensures we don't waste any cycles rendering
       *  frames that will never be displayed to the screen.
       */

      imGuiRender();
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
  bool m_shouldDraw{true};
  bool m_shouldClose{false};

  template <class TWindow>
  friend int __stdcall PeekRun(TWindow &&window)
  {
    if (window.m_shouldClose)
    {
      Log<File>::Write(L"App Creation canceled");
      return 1;
    };

#ifndef IMGUI_DISABLE
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(window.m_Handle);
    ImGui_ImplDX11_Init(window.m_pDeviceResource->GetDevice().Get(), window.m_pContext.Get());

#endif
    ::MSG messages{};
    while (messages.message != WM_QUIT)
    {
      ::PeekMessageW(&messages, 0, 0, 0, PM_REMOVE);
      ::TranslateMessage(&messages);
      ::DispatchMessageW(&messages);

      if (window.m_shouldDraw)
        window.App::Draw();
    };
    ImGui_ImplWin32_Shutdown();
    return 0;
  };
};

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
int wWinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) { return Invoke(&App::AppEntry, hinst); };
int main() { return Invoke(&App::AppEntry, (HINSTANCE)&__ImageBase); };