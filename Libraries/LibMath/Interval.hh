#pragma once

#include <VS/Types.hh>

namespace Uni
{

template <typename Type = f64>
struct Interval
{
  Type left;
  Type right;

  [[nodiscard]] constexpr auto contains(Type x) const -> bool
  {
    return left <= x && x <= right;
  }
};

}  // namespace Uni
