#ifndef MAIN_ENGINE_H_
#define MAIN_ENGINE_H_

#include <boost/leaf.hpp>
#include <memory>
#include <log.h>
#include <window.h>
#include <renderer.h>
#include <resource.h>

class Engine {
 public:
  static boost::leaf::result<Engine> Create(int argc, char* argv[]);
  boost::leaf::result<void> Run();

  Engine(const Engine& engine) = delete;
  Engine& operator=(const Engine& engine) = delete;
  Engine(Engine&& engine) = default;
  Engine& operator=(Engine&& engine) = default;

  void ProcessEvents();

 private:
  static bool is_initialized;
  Engine() : quit_{false} { 
  }

  bool quit_;

  std::unique_ptr<core::ILogger> logger_;
  std::unique_ptr<core::Window> window_;
  std::unique_ptr<render::Renderer> renderer_;
  std::unique_ptr<core::ResourceManager> resource_manager_ = std::make_unique<core::ResourceManager>(std::filesystem::current_path());
};

#endif