#pragma once

#include <VS/Types.hh>

namespace Uni
{

template <typename Type = f64>
struct Point
{
  constexpr explicit Point() = default;
  constexpr explicit Point(Type x, Type y) : x(x), y(y) {}

  Type x;
  Type y;
};

}  // namespace Uni
