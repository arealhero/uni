#pragma once

#include <LibInterpolation/Spline/Interpolator.hh>

namespace Uni
{

class QuadraticSplineInterpolator : public SplineInterpolator
{
 public:
  std::string get_name() const override
  {
    return std::string{"Quadratic spline (2,1)"};
  }

  Spline operator()(std::vector<Point> const& points) override;
};

}  // namespace Uni

