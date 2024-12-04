#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <boost/signals2.hpp>

namespace zge {

template <typename Signature>
class Signal;

template <typename Signature>
class Slot;

template <typename F, typename... Args>
class Slot<F(Args...)> {
 public:
  using Signature = F(Args...);
  friend class Signal<Signature>;

  template<typename Signature1>
  Slot(Signature1 func) : slot_(func) {}
 private:
  boost::signals2::slot<Signature> slot_;
};

template<typename F, typename... Args>
class Signal<F(Args...)> {
 public:
  using Signature = F(Args...);

  void Connect(Slot<Signature> slot) { 
    signal_.connect(slot.slot_);
  }
  void Emit(Args&&... args) { 
    signal_(std::forward<Args>(args)...);
  }
 private:
  boost::signals2::signal<Signature> signal_;
};

}

#endif