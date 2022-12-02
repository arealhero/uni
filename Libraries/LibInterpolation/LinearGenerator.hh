#pragma once

#include <LibInterpolation/SequenceGenerator.hh>

namespace Uni
{

template <typename ValueType = double>
class LinearGenerator : public SequenceGenerator<ValueType>
{
 public:
  using SequenceGenerator<ValueType>::SequenceGenerator;

  ValueType get(std::size_t index) const override
  {
    return this->a + index * step();
  }

  constexpr ValueType step() const
  {
    return (this->b - this->a) / (this->n - 1);
  }
};

}  // namespace Uni
