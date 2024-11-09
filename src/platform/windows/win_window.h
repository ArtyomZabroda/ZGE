#ifndef PLATFORM_WINDOWS_WIN_WINDOW_H_
#define PLATFORM_WINDOWS_WIN_WINDOW_H_

#include <window.h> // our window interface class is here
#include <windows.h> // windows.h from winapi

namespace zge {

/// <summary>
/// Implementation of the IWindow interface for the Windows platform
/// </summary>
class WinWindow : public IWindow {
 public:
  WinWindow(int width, int height); 
 private:
  HWND hwnd_;

  // Унаследовано через IWindow
  void Show() const override;
  void Hide() const override;
  Extent2D Extent() const override;
  void SetExtent(Extent2D value) override;
};

}

#endif