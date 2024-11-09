#ifndef PLATFORM_WINDOW_H_
#define PLATFORM_WINDOW_H_

#include <util/extent.h>

namespace zge {


/// <summary>
/// Exposes a window interface, which is implemented by every supported platform
/// </summary>
class IWindow {
 public:
  /// <summary>
  /// Shows the window
  /// </summary>
  virtual void Show() const = 0;

  /// <summary>
  /// Hides the window
  /// </summary>
  virtual void Hide() const = 0;

  /// <summary>
  /// Gets the extent (size) of the window
  /// </summary>
  /// <returns>
  /// Two-dimensional extent of the window measured in pixels
  /// </returns>
  virtual Extent2D Extent() const = 0;

  /// <summary>
  /// Sets the extent of the window
  /// </summary>
  /// <param name="value">The new two-dimensional extent of the window measured in pixels</param>
  virtual void SetExtent(Extent2D value) = 0;

  virtual ~IWindow() = default;
};

}

#endif