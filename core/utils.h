#ifndef CORE_UTILS_H_
#define CORE_UTILS_H_

#include <boost/leaf.hpp>
#include <cstddef>

namespace core {

struct Extent2D {
  std::size_t width, height;
};

template<typename T>
class Result : boost::leaf::result<T> {
  template <class TryBlock, class... H>
  friend inline
      typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
      TryHandleAll(TryBlock&& try_block, H&&... h);

 public:
  operator boost::leaf::result<T>() const { return this; }

};

template <class TryBlock, class... H>
inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
TryHandleAll(TryBlock&& try_block, H&&... h) {
  return boost::leaf::try_handle_all(std::forward<TryBlock>(try_block),
                                     std::forward<H>(h)...);
}

using ErrorID = boost::leaf::error_id;

inline ErrorID NewError() noexcept {
  return boost::leaf::new_error();
}

template <class... Item>
inline ErrorID NewError(Item&&... item) noexcept {
  return boost::leaf::new_error(std::forward<Item>(item)...);
}

}

#endif