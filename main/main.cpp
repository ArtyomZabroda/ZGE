#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <log.h>

int main(int argc, char* argv[]) {
  core::SingleThreadedLogger st_logger;
  if (SDL_Init(SDL_INIT_VIDEO)) {
    st_logger.LogTrace("SDL initialized successfully.");
  } else {
    st_logger.LogFatal(
        std::format("Failed to initialize SDL: {}", SDL_GetError()));
    
  }
	return 0;
}