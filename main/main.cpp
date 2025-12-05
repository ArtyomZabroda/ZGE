#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "engine.h"

int main(int argc, char* argv[]) {
  int exit_code = boost::leaf::try_handle_all(
                 [&argc, &argv]() -> boost::leaf::result<int> {
                   BOOST_LEAF_AUTO(engine, Engine::Create(argc, argv));
                   BOOST_LEAF_CHECK(engine.Run());
                   return EXIT_SUCCESS;
                 },
                 []() -> int {
                   return EXIT_FAILURE;
                 });
  return exit_code;
}