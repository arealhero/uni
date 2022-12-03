#pragma once

#include <LibInterpolation/Polynomial/Interpolator.hh>

namespace Uni
{

class LagrangeInterpolator final : public PolynomialInterpolator
{
 public:
  constexpr std::string get_name() const override { return "Lagrange"; }

  Polynomial operator()(const std::vector<Point>& points) override;
};

}  // namespace Uni
