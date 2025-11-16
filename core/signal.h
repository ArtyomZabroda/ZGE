#ifndef CORE_SIGNAL_H_
#define CORE_SIGNAL_H_

#include <vector>
#include <functional>

namespace core {

template<typename Signature>
class Signal;

template<typename F, typename... Args>
class Signal<F(Args...)> {
 public:
  template<typename Signature>
  void Connect(Signature slot) {
    slots_.push_back(slot);
  }
  void Emit(Args&&... args) {
    for (std::function<F(Args...)>& slot : slots_) {
      slot(args...);
    }
  }

 private:
  std::vector<std::function<F(Args...)>> slots_;
};

}

#endif