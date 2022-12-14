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

  [[nodiscard]] auto get(std::size_t index) const -> ValueType override
  {
    constexpr auto pi = std::numbers::pi;
    const double a = this->a;
    const double b = this->b;
    const double n = this->n;

    const auto cos =
        std::cos(pi * static_cast<double>(2 * index + 1) / (2 * n));

    return ((a - b) * cos + b + a) / 2;
  }
};

}  // namespace Uni
