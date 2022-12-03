#pragma once

#include <LibInterpolation/Spline/Interpolator.hh>

namespace Uni
{

class CubicSplineInterpolator : public SplineInterpolator
{
 public:
  std::string get_name() const override
  {
    return std::string{"Cubic spline (3,2)"};
  }

  Spline operator()(std::vector<Point> const& points) override;
};

}  // namespace Uni
