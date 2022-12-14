#pragma once

#include <LibInterpolation/SequenceGenerator.hh>

namespace Uni
{

template <typename ValueType = double>
class LinearGenerator : public SequenceGenerator<ValueType>
{
 public:
  using SequenceGenerator<ValueType>::SequenceGenerator;

  [[nodiscard]] auto get(std::size_t index) const -> ValueType override
  {
    return this->a + index * step();
  }

  [[nodiscard]] constexpr auto step() const -> ValueType
  {
    return (this->b - this->a) / (this->n - 1);
  }
};

}  // namespace Uni
