#pragma once

#include <LibInterpolation/Spline/Interpolator.hh>

namespace Uni
{

class CubicSplineInterpolator : public SplineInterpolator
{
 public:
  [[nodiscard]] auto get_name() const -> std::string override
  {
    return std::string{"Cubic spline (3,2)"};
  }

  [[nodiscard]] auto operator()(std::vector<Point> const& points)
      -> Spline override;
};

}  // namespace Uni
