#pragma once

#include <LibInterpolation/SequenceGenerator.hh>
#include <cmath>
#include <numbers>

namespace Uni
{

template <typename ValueType = double>
class OptimalGenerator : public SequenceGenerator<ValueType>
{
 public:
  using SequenceGenerator<ValueType>::SequenceGenerator;

  ValueType get(std::size_t index) const override
  {
    constexpr auto pi = std::numbers::pi;
    const double a = this->a;
    const double b = this->b;
    const double n = this->n;

    const auto cos = std::cos(pi * (2 * index + 1) / (2 * n));

    return ((a - b) * cos + b - a) / 2;
  }
};

}  // namespace Uni
