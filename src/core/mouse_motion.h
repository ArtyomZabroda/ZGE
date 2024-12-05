#include <cstdint>
#ifndef CORE_MOUSE_MOTION_H_
#define CORE_MOUSE_MOTION_H_

namespace zge {

struct MouseMotion {
  uint32_t x_pos, y_pos;
  int x_rel, y_rel;
};

}

#endif