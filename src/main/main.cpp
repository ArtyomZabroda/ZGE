#include <cerrno>
#include <memory>

#include <win_window.h>

namespace zge {

  /// <summary>
  /// Engine's main function. It should be called from platform-specific main function (wWinMain on Windows, for example)
  /// </summary>
  /// <param name="argc">Number of parameters</param>
  /// <param name="argv">Pointer to an array of c-style strings</param>
  /// <returns>Exit code</returns>
  int Main(int argc, char** argv) {
    std::unique_ptr<IWindow> window = std::make_unique<WinWindow>(Extent2D{.width = 1600, .height = 900});
    window->Show();
    while (true) {
      window->ProcessInput();
    }
    return EXIT_SUCCESS;
  }

}