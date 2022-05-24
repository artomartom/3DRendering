
#pragma once

#include "../pch.hpp"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuiMngr.hpp"
#include "../Hello/CoreWindow.hpp"
#include <tuple>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace IMGUI
{

    using namespace ::Window;
    struct packedCreateArgs_t
    {
        IMGUI *pthis{};
        CreationArgs args{};
    };
    LRESULT CALLBACK IMGUI::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        IMGUI *pThis{(IMGUI *)::GetWindowLongPtrW(hwnd, -21)}; // GWL_USERDATA

        if (::ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        {
            return true;
        }

        switch (message)
        {
        case WM_CREATE:
        {
            packedCreateArgs_t *args = reinterpret_cast<packedCreateArgs_t *>((reinterpret_cast<::CREATESTRUCT *>(lParam))->lpCreateParams); // setwindowlong isnt called
            args->pthis->OnCreate(args->args);
            SetWindowLongPtrW(hwnd, -21, (LONG_PTR)args->pthis); // GWL_USERDATA
            ::ShowWindow(hwnd, SW_SHOWDEFAULT);
            ::UpdateWindow(hwnd);
            return false;
        }
        case WM_SIZE:
            pThis->OnSizeChanged({wParam, lParam});
            return false;

        case WM_CLOSE:
            pThis->m_done = true;
            return 0;
        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }
    }
    void IMGUI::OnCreate(_In_ const CreationArgs &args) noexcept
    {
        m_rect = args.rect;
    };
    void IMGUI::OnSizeChanged(_In_ const SizeChangedArgs &args) noexcept {};

    IMGUI::IMGUI(ID3D11Device *pDevice, ID3D11DeviceContext *pContext, HWND parentWindowHandle, const ViewPort &windowViewPort)
    {

        if (pDevice == nullptr || pContext == nullptr)
            return;

        WNDCLASSEXW wndclass{};
        wndclass.cbSize = sizeof(WNDCLASSEXW);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbWndExtra = sizeof(HWND);
        wndclass.hInstance = GetModuleHandle(0);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.lpszClassName = m_childClsName;

        if (!::RegisterClassExW(&wndclass))
        {
            Error<File>::Write(L"RegisterClassExW failed");
            return;
        }

        RECT rectGui{};
        GetWindowRect(parentWindowHandle, &rectGui);
        packedCreateArgs_t args{
            this,
            {},
        };
        HWND m_handle = ::CreateWindowExW(0, m_childClsName, L"IMGUI",
                                          WS_CHILD | WS_VISIBLE /*| WS_CAPTION | WS_SYSMENU | WS_THICKFRAME*/,
                                          0, 0, RECTWIDTH(rectGui), 300,
                                          parentWindowHandle,
                                          NULL, GetModuleHandleW(0), &args);

        if (m_handle == 0)
        {
            Error<File>::Write(L"CreateWindowExW failed");
            return;
        };

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        // ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();
        ImGui_ImplWin32_Init(m_handle);
        ImGui_ImplDX11_Init(pDevice, pContext);
        m_ready = true;

        Log<File>::Write(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
    };

    IMGUI::~IMGUI()
    {
        if (m_handle != 0)
        {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            ::DestroyWindow(m_handle);
            ::UnregisterClassW(m_childClsName, 0);
        }
    };
};