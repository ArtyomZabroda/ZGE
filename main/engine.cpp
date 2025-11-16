#include "engine.h"
#include <SDL3/SDL.h> 
#include <signal.h>

boost::leaf::result<Engine> Engine::Create(int argc, char* argv[]) {
  Engine engine;
  core::SingleThreadedLogger::Init();
  engine.logger_ = std::make_unique<core::SingleThreadedLogger>();

  if (SDL_Init(SDL_INIT_VIDEO)) {
    engine.logger_->LogTrace("SDL initialized successfully.");
  } else {
    engine.logger_->LogFatal(
        std::format("Failed to initialize SDL: {}", SDL_GetError()));
    return boost::leaf::new_error();
  }

  boost::leaf::result<std::unique_ptr<core::Window>> window = boost::leaf::try_handle_some(
      []() -> boost::leaf::result<std::unique_ptr<core::Window>> {
            BOOST_LEAF_AUTO(window,
                            core::Window::Create(core::Extent2D(1280, 720),
                                                 "Hello, world!"));
            return std::make_unique<core::Window>(std::move(window));
      },
          [&engine](core::WindowError err, const std::string& msg)
              -> boost::leaf::result<std::unique_ptr<core::Window>> {
        engine.logger_->LogFatal(msg);
        return boost::leaf::new_error();
      });

  BOOST_LEAF_ASSIGN(engine.window_, std::move(window));

  return engine;
}

boost::leaf::result<void> Engine::Run() {
  while (!quit_) {
    ProcessEvents();
  }
  return {};
}

void Engine::ProcessEvents() { 
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        quit_ = true;
        break;
    }
  }
}
