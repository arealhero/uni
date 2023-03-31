#pragma once

#include <LibInterpolation/Polynomial/Interpolator.hh>

namespace Uni
{

class NewtonInterpolator final : public PolynomialInterpolator
{
 public:
  [[nodiscard]] constexpr auto get_name() const -> std::string override
  {
    return std::string{"Newton"};
  }

  [[nodiscard]] auto operator()(const std::vector<Point<f64>>& points)
      -> Polynomial override;
};

}  // namespace Uni
