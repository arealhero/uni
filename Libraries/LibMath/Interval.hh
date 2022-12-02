#pragma once

namespace Uni
{

struct Interval
{
  double left;
  double right;

  constexpr bool contains(double x) const { return left <= x && x <= right; }
};

}  // namespace Uni
