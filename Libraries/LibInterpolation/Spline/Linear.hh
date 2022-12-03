#pragma once

#include <LibInterpolation/Spline/Interpolator.hh>

namespace Uni
{

class LinearSplineInterpolator : public SplineInterpolator
{
 public:
  std::string get_name() const override
  {
    return std::string{"Linear spline (1,0)"};
  }

  Spline operator()(std::vector<Point> const& points) override;
};

}  // namespace Uni
