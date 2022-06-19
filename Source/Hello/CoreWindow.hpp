
#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define RECTWIDTH(x) (x.right - x.left)
#define RECTHEIGHT(x) (x.bottom - x.top)

namespace Window
{

#pragma region EventArgs
  struct CreationArgs
  {
    /*args*/
    ::HINSTANCE hInst{};
    ::RECT rect{};
  };

  struct SizeChangedArgs
  {

    SizeChangedArgs(::WPARAM wParam, ::LPARAM lParam)
        : changeType{static_cast<enum class ChangeType>(wParam)}, newSize{LOWORD(lParam), HIWORD(lParam)} {};
    /*args*/
    enum class ChangeType : ::WPARAM
    {
      MaxHide = SIZE_MAXHIDE,     // Message is sent to all pop-up windows when some other window is maximized.
      Maximized = SIZE_MAXIMIZED, // The window has been maximized.
      MaxShow = SIZE_MAXSHOW,     // Message is sent to all pop-up windows when some other window has been restored to its former size.
      Minimized = SIZE_MINIMIZED, // The window has been minimized.
      Restored = SIZE_RESTORED,   // The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.

    } changeType{};

    struct Size
    {
      long width, height;
    } newSize{};
  };

  struct AppEventArgs
  {
    AppEventArgs(::UINT message, ::WPARAM wParam, ::LPARAM lParam)
        : wParam{wParam}, lParam{lParam}, message{message} {};
    ::WPARAM wParam{};
    ::LPARAM lParam{};
    ::ULONG message{};
  };

  struct CursorArgs
  {
    CursorArgs() = delete;
    enum class Event : UINT;
    CursorArgs(Event event, ::WPARAM wParam, ::LPARAM lParam)
        : event{event},
          misc{static_cast<enum class Misc>(wParam)},
          pos{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)} {};

    enum class Event : UINT
    {
      None = (0u),
      Move = (1u << 0),
      // action
      Up = (1u << 2),
      Down = (1u << 3),
      DoubleClick = (1u << 4),
      // button
      Left = (1u << 5),
      Middle = (1u << 6),
      Right = (1u << 7),
      // Left
      LUp = (Left | Up),
      LDown = (Left | Down),
      LDoubClick = (Left | DoubleClick),
      // Middle
      MUp = (Middle | Up),
      MDown = (Middle | Down),
      MDoubClick = (Middle | DoubleClick),
      // Right
      RUp = (Right | Up),
      RDown = (Right | Down),
      RDoubClick = (Right | DoubleClick),

    } event{};
    enum class Misc : ::WPARAM
    {
      //... during primary event
      None = 0,
      CtrlDown = MK_CONTROL, // The CTRL key is down.
      LBDown = MK_LBUTTON,   // The left Cursor button is down.
      MBDown = MK_MBUTTON,   // The middle Cursor button is down.
      RBDown = MK_RBUTTON,   // The right Cursor button is down.
      ShiftDown = MK_SHIFT,  // The SHIFT key is down.
      X1Down = MK_XBUTTON1,  // The first X button is down.
      X2Down = MK_XBUTTON2,  // The second X button is down.
    } misc{};
    ::POINT pos{};
  };
  // comparison for  enum classes
  template <typename T>
  T operator&(T &left, T &right)
  {
    using UT = std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(left) & static_cast<UT>(right));
  };
  template <typename T>
  T operator|(T &left, T &right)
  {
    using UT = std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(left) | static_cast<UT>(right));
  };

  template <typename T>
  auto operator!=(T &left, T &right)
  {
    using UT = std::underlying_type<T>::type;
    return (static_cast<UT>(left) != static_cast<UT>(right));
  };
  template <typename T>
  bool operator==(T &left, T &right)
  {
    using UT = std::underlying_type<T>::type;
    return (static_cast<UT>(left) == static_cast<UT>(right));
  };

  struct CommandArgs
  {
    CommandArgs(::WPARAM wParam, ::LPARAM lParam)
        : wParam{wParam}, lParam{lParam} {};
    ::WPARAM wParam{};
    ::LPARAM lParam{};
  };

  struct KeyEventArgs
  {
    KeyEventArgs(::WPARAM wParam)
        : virtualKey{wParam} {};
    ::WPARAM virtualKey{}; // Key Code   ...
  };

  struct ActivateArgs
  {
    ActivateArgs(WPARAM wParam)
        : activateType{LOWORD(wParam)},
          isMinimized{static_cast<bool>(HIWORD(wParam))} {};
    /*args*/
    enum class ActivateType : ::WORD
    {
      Activated = WA_ACTIVE,           // Activated by some method other than a Cursor click (  by a call to the SetActiveWindow function or by use of the keyboard interface to select the window).
      ClickActivated = WA_CLICKACTIVE, // Activated by a Cursor click.
      Deactivated = WA_INACTIVE,       // Deactivated.
    } activateType{};
    bool isMinimized{};
  };

#pragma endregion // EventArgs

  template <class TImpl>
  class CoreWindow : public TImpl
  {

  public:
    CoreWindow() = delete;
    CoreWindow(const CoreWindow &) = delete;
    CoreWindow(CoreWindow &&) = delete;
    CoreWindow &operator=(const CoreWindow &) = delete;
    CoreWindow &operator=(CoreWindow &&) = delete;
    ~CoreWindow()
    {
      TImpl::OnClose();
      ::DestroyWindow(TImpl::m_handle);
    };
    struct packedCreateArgs_t
    {
      CoreWindow<TImpl> *pthis{};
      CreationArgs args{};
    };
    CoreWindow(HINSTANCE hinst, const RECT rect)
    {

      WNDCLASSEXW wincl{
          sizeof(WNDCLASSEXW),
          CS_GLOBALCLASS | CS_PARENTDC,
          CoreProcedure,
          0, 0, GetModuleHandle(NULL), 0, ::LoadCursorA(0, IDC_ARROW), 0, 0,
          L"CoreWindow", 0};
      ::RegisterClassExW(&wincl);

      RECT MainRect{rect};
      ::AdjustWindowRectEx(&MainRect, TImpl::m_style, 0, TImpl::m_styleEx);

      packedCreateArgs_t createArgsTpl{this, CreationArgs{hinst, rect}};

      ::CreateWindowExW(
          TImpl::m_styleEx,
          wincl.lpszClassName, 0,
          TImpl::m_style,
          MainRect.left, MainRect.top,
          RECTWIDTH(MainRect),
          RECTHEIGHT(MainRect),
          HWND_DESKTOP, nullptr, nullptr, &createArgsTpl);
    };

  private:
    template <class CoreWindowSpec>
    friend int __stdcall Run(const CoreWindowSpec &window);

      static ::LRESULT __stdcall CoreProcedure(::HWND hWnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
    {

#define PROCCASE(message, boolreturn, action) \
  case message:                               \
    action;                                   \
    return boolreturn

      CoreWindow *pthis{(CoreWindow *)::GetWindowLongPtrW(hWnd, -21)}; // GWL_USERDATA
      switch (message)
      {
        /*window status */
        PROCCASE(WM_CREATE, false,
                 {
                   CREATESTRUCT *createStruct{(reinterpret_cast<::CREATESTRUCT *>(lParam))};
                   auto *tupl{reinterpret_cast<packedCreateArgs_t *>(createStruct->lpCreateParams)};
                   pthis = tupl->pthis; // needed to call OnCreate
                   pthis->TImpl::m_handle = hWnd;
                   ::SetWindowLongPtrW(pthis->TImpl::m_handle, -21, (LONG_PTR)pthis); // GWL_USERDATA
                   pthis->TImpl::OnCreate(tupl->args);
                   ::ShowWindow(hWnd, SW_SHOWDEFAULT);
                   ::UpdateWindow(hWnd);
                 });
        PROCCASE(WM_ACTIVATE, false, { pthis->TImpl::OnWindowActivate({wParam}); });
        PROCCASE(WM_CLOSE, false, { ::PostQuitMessage(0); });
        /*visual*/
        PROCCASE(WM_ERASEBKGND, true, {});
        PROCCASE(WM_PAINT, false,
                 {
                   ::ValidateRect(hWnd, 0);
                   pthis->TImpl::OnPaint();
                 };);
        /*window size*/
        PROCCASE(WM_SIZE, false, pthis->TImpl::OnSizeChanged({wParam, lParam}));
        PROCCASE(WM_SIZING, true, pthis->TImpl::OnSizing(reinterpret_cast<RECT *>(lParam)));
        PROCCASE(WM_ENTERSIZEMOVE, false, { ::Sleep(30); });
        PROCCASE(WM_EXITSIZEMOVE, false, { ::Sleep(30); });

#pragma region user_input
        PROCCASE(WM_MOUSEMOVE, false, pthis->OnCursorMove(CursorArgs{CursorArgs::Event::Move, wParam, lParam}));
        // left button
        PROCCASE(WM_LBUTTONUP, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::LUp, wParam, lParam)));
        PROCCASE(WM_LBUTTONDOWN, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::LDown, wParam, lParam)));
        PROCCASE(WM_LBUTTONDBLCLK, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::LDoubClick, wParam, lParam)));
        // middle button
        PROCCASE(WM_MBUTTONUP, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::MUp, wParam, lParam)));
        PROCCASE(WM_MBUTTONDOWN, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::MDown, wParam, lParam)));
        PROCCASE(WM_MBUTTONDBLCLK, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::MDoubClick, wParam, lParam)));
        // right button
        PROCCASE(WM_RBUTTONUP, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::RUp, wParam, lParam)));
        PROCCASE(WM_RBUTTONDOWN, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::RDown, wParam, lParam)));
        PROCCASE(WM_RBUTTONDBLCLK, false, pthis->OnCursorEvent(CursorArgs(CursorArgs::Event::RDoubClick, wParam, lParam)));
        // keyboard
        PROCCASE(WM_KEYDOWN, false, { (!(lParam & (1LL << 31))) ? pthis->OnKeyStroke({wParam}) : pthis->OnKeyHold({wParam}); });
        // misc
        PROCCASE(WM_COMMAND, false, { pthis->TImpl::OnCommand(CommandArgs(wParam, lParam)); });
#pragma region end // user_input
      };

      if (message > WM_APP)
      {
        pthis->TImpl::OnAppEvent(AppEventArgs(message, wParam, lParam));
        return 0;
      }
      else
      {
        return ::DefWindowProcW(hWnd, message, wParam, lParam);
      };

#undef PROCCASE
    };
  };

  template <class CoreWindowSpec>
  int __stdcall Run(const CoreWindowSpec &window)
  {
    ::MSG messages{};

    while (::GetMessageW(&messages, 0, 0, 0))
    {

      ::TranslateMessage(&messages);
      ::DispatchMessageW(&messages);
    };

    return 0;
  };
};

class CoreApp
{
public:
  /**
   * Event proc message handlers
   */
  void OnCreate(_In_ const ::Window::CreationArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnPaint() noexcept {/*UNREFERENCED_PARAMETER(args);*/};
  void OnClose() noexcept {/*UNREFERENCED_PARAMETER(args);*/};
  void OnKeyHold(_In_ const ::Window::KeyEventArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnKeyStroke(_In_ const ::Window::KeyEventArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnCursorMove(_In_ const ::Window::CursorArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnCursorEvent(_In_ const ::Window::CursorArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnSizing(_Inout_updates_opt_(1) RECT *pRect) noexcept { UNREFERENCED_PARAMETER(pRect); };
  void OnCommand(_In_ const ::Window::CommandArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnSizeChanged(_In_ const ::Window::SizeChangedArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnWindowActivate(_In_ const ::Window::ActivateArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };
  void OnAppEvent(_In_ const ::Window::AppEventArgs &args) noexcept { UNREFERENCED_PARAMETER(args); };

  // implementation calls this function to close itself
  void Close() noexcept { ::SendMessageW(m_handle, WM_CLOSE, 0, 0); };

  bool IsValid() noexcept { return m_handle != 0; };

protected:
  HWND m_handle{nullptr};
  bool m_isVisible{};
  DWORD m_styleEx{WS_EX_OVERLAPPEDWINDOW};
  DWORD m_style{WS_OVERLAPPED |
                WS_CAPTION |
                WS_SYSMENU |
                WS_MINIMIZEBOX |
                WS_THICKFRAME |
                WS_MAXIMIZEBOX |
                WS_CAPTION |
                WS_MINIMIZE};
};

#endif
