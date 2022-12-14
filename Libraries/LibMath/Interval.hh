#pragma once

namespace Uni
{

struct Interval
{
  double left;
  double right;

  [[nodiscard]] constexpr auto contains(double x) const -> bool
  {
    return left <= x && x <= right;
  }
};

}  // namespace Uni
