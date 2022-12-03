#pragma once

#include <LibInterpolation/Polynomial/Interpolator.hh>

namespace Uni
{

class NewtonInterpolator final : public PolynomialInterpolator
{
 public:
  constexpr std::string get_name() const override
  {
    return std::string{"Newton"};
  }

  Polynomial operator()(const std::vector<Point>& points) override;
};

}  // namespace Uni
