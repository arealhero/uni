#pragma once

namespace Uni
{

class Function
{
 public:
  virtual ~Function() = default;

  [[nodiscard]] virtual double operator()(double x) const = 0;
};

}  // namespace Uni
