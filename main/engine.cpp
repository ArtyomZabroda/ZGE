#include "engine.h"
#include <SDL3/SDL.h> 
#include <signal.h>

boost::leaf::result<Engine> Engine::Create(int argc, char* argv[]) {
  Engine engine;
  engine.logger_ = std::make_unique<core::SingleThreadedLogger>();

  if (SDL_Init(SDL_INIT_VIDEO)) {
    engine.logger_->LogTrace("SDL initialized successfully.");
  } else {
    engine.logger_->LogFatal(
        std::format("Failed to initialize SDL: {}", SDL_GetError()));
    return boost::leaf::new_error();
  }

  boost::leaf::result<std::unique_ptr<core::Window>> window = 
      boost::leaf::try_handle_some(
          [&engine]() -> boost::leaf::result<std::unique_ptr<core::Window>> {
            BOOST_LEAF_AUTO(window,
                            core::Window::Create(engine.logger_.get(),
                                                 core::Extent2D(1280, 720),
                                                 "Hello, world!"));
            return std::make_unique<core::Window>(std::move(window));
          },
          [&engine](core::WindowError err, const std::string& msg)
              -> boost::leaf::result<std::unique_ptr<core::Window>> {
            return boost::leaf::new_error();
          });
  BOOST_LEAF_ASSIGN(engine.window_, std::move(window));

  boost::leaf::result<std::unique_ptr<render::Renderer>> renderer =
      boost::leaf::try_handle_some(
          [&engine]() -> boost::leaf::result<std::unique_ptr<render::Renderer>> {
            auto&& [extensions, size] = engine.window_->GetVulkanInstanceExtensions();
            BOOST_LEAF_AUTO(renderer,
                            render::Renderer::Create(engine.logger_.get(), engine.window_.get(), std::span<const char* const>(extensions, size)));
            return std::make_unique<render::Renderer>(std::move(renderer));
          });
  BOOST_LEAF_ASSIGN(engine.renderer_, std::move(renderer));

  return engine;
}

boost::leaf::result<void> Engine::Run() {
  while (!quit_) {
    ProcessEvents();
    renderer_->DrawFrame();
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
