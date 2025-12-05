#ifndef RENDER_RENDER_UTILS_H_
#define RENDER_RENDER_UTILS_H_

#include <vulkan/vulkan_raii.hpp>
#include <boost/leaf.hpp>

#include "log.h"
#include <cstdint>

// This macro either converts ValueResult to an underlying value or returns an
// error for boost leaf
#define ZGE_VK_ASSIGN(v, r)                               \
  auto&& BOOST_LEAF_TMP = r;                              \
  if (!BOOST_LEAF_TMP.has_value())                        \
    return boost::leaf::new_error(BOOST_LEAF_TMP.result); \
  v = std::forward<decltype(BOOST_LEAF_TMP)>(BOOST_LEAF_TMP).value

// Unfolds the ValueResult r in new variable with name v
#define ZGE_VK_AUTO(v, r) ZGE_VK_ASSIGN(auto v, r)

#endif