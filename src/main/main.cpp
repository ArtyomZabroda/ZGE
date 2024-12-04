#include <iostream>
#include <window.h>
#include <renderer.h>
#include <signal.h>
#include <log.h>

namespace zge {

int ZMain(int argc, char** argv) {
  InitLogging(argv[0]);
  Window window("ZGAMEENGINE", Extent2D{.width = 1600, .height = 900});
  Renderer renderer(&window);
  window.Show();
  bool window_closed = false;
  window.Closed().Connect([&window_closed]() { window_closed = true; });
  while (!window_closed) {
    window.ProcessInput();
    renderer.DrawFrame();
  }
  return 0;
}

}
