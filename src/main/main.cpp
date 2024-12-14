#include <engine.h>

namespace zge {

int ZMain(int argc, char** argv) {
  zge::Engine engine(argc, argv);
  return engine.Run();
}

}
