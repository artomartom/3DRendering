
#pragma once
#include "imgui.h"

// TODO :imgui port size params
// TODO :resizing handler

IMGUI_IMPL_API void ImGui_ImplDX11_RenderDrawData(ImDrawData *draw_data);
IMGUI_IMPL_API void ImGui_ImplWin32_NewFrame();
IMGUI_IMPL_API void ImGui_ImplDX11_NewFrame();
namespace IMGUI
{

    // child window's size in normalized coordinates
    struct ViewPort
    {

        float left, top, right, bottom;
    };

    class IMGUI
    {
    public:
        IMGUI() = default;
        ~IMGUI();

        IMGUI(ID3D11Device *pDevice, ID3D11DeviceContext *pContext, HWND parentWindowHandle, const ViewPort &windowViewPort = {0.1f, 0.2f, 0.2f, 0.2f});

        template <typename Func>
        void RenderFrame(Func func)
        {
          
            
            static bool show_demo_window{};
            using namespace ImGui;
            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            NewFrame();

            SetNextWindowSize(ImVec2( m_rect.right, m_rect.bottom));
            Log<File>::Write(m_rect.left,m_rect.top, m_rect.right, m_rect.bottom);
            func();
            //GetWindowSize();
            //auto Viewport{GetMainViewport()};
            //Log<File>::Write(Viewport->Pos.x, Viewport->Pos.y);
            //Log<File>::Write(Viewport->Size.x, Viewport->Size.y);
            //auto lol{GetContentRegionAvail()};
            //Log<File>::Write(lol.x, lol.y);
            //Log<File>::Write(" ");
            Render();
            ImGui_ImplDX11_RenderDrawData(GetDrawData());
        };

    private:
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        void  OnCreate(_In_ const ::Window::CreationArgs &args) noexcept ;
        void  OnSizeChanged(_In_ const ::Window::SizeChangedArgs &args) noexcept  ;
        
    private:
        bool m_done{false};
        bool m_ready{false};

        HWND m_handle{};
        RECT m_rect{};
        LPCWSTR m_childClsName{L"IMGUI class"};
    };

};