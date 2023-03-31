#pragma once

#include <LibInterpolation/Spline/Interpolator.hh>

namespace Uni
{

class QuadraticSplineInterpolator : public SplineInterpolator
{
 public:
  [[nodiscard]] auto get_name() const -> std::string override
  {
    return std::string{"Quadratic spline (2,1)"};
  }

  [[nodiscard]] auto operator()(std::vector<Point<f64>> const& points)
      -> Spline override;
};

}  // namespace Uni
