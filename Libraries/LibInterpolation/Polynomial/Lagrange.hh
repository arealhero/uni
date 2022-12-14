#pragma once

#include <LibInterpolation/Polynomial/Interpolator.hh>

namespace Uni
{

class LagrangeInterpolator final : public PolynomialInterpolator
{
 public:
  [[nodiscard]] constexpr auto get_name() const -> std::string override
  {
    return "Lagrange";
  }

  [[nodiscard]] auto operator()(const std::vector<Point>& points)
      -> Polynomial override;
};

}  // namespace Uni
