#pragma once

namespace Uni
{

class Function
{
 public:
  virtual ~Function() = default;

  [[nodiscard]] virtual auto operator()(double x) const -> double = 0;
};

}  // namespace Uni
