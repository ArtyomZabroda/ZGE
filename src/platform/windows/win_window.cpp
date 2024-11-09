#include "win_window.h"

zge::WinWindow::WinWindow(int width, int height)
{
  const wchar_t kWndClassName[] = L"zge";

  WNDCLASS wc{};
  wc.lpfnWndProc = DefWindowProc;
  wc.hInstance = GetModuleHandle(0); // GetModuleHandle(0) returns hInstance from winmain
  wc.lpszClassName = kWndClassName;

  RegisterClass(&wc);

  hwnd_ = CreateWindowEx(
    0,                              // Optional window styles
    kWndClassName,                   // Window class
    L"Learn to Program Windows",    // Window text
    WS_OVERLAPPEDWINDOW,            // Window style

    // Size and position
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

    NULL,       // Parent window    
    NULL,       // Menu
    wc.hInstance,  // Instance handle
    this        // Additional application data. We are passing a pointer to "this", which we will use in a WindowProc
  );
}

void zge::WinWindow::Show() const
{
  ShowWindow(hwnd_, SW_SHOWDEFAULT);
}

void zge::WinWindow::Hide() const
{
  ShowWindow(hwnd_, SW_HIDE);
}

zge::Extent2D zge::WinWindow::Extent() const
{
  RECT rect;
  GetWindowRect(hwnd_, &rect);
  return Extent2D{ .width = static_cast<std::size_t>(rect.bottom - rect.top), .height = static_cast<std::size_t>(rect.right - rect.left) };
}

void zge::WinWindow::SetExtent(Extent2D value)
{

}
