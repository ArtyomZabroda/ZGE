#ifndef MAIN_ENGINE_H_
#define MAIN_ENGINE_H_

#include <boost/leaf.hpp>
#include <memory>
#include <log.h>
#include <window.h>

class Engine {
 public:
  static boost::leaf::result<Engine> Create(int argc, char* argv[]);
  boost::leaf::result<void> Run();

  void ProcessEvents();

 private:
  Engine() : quit_{false} {}

  bool quit_;

  std::unique_ptr<core::ILogger> logger_;
  std::unique_ptr<core::Window> window_;
};

#endif