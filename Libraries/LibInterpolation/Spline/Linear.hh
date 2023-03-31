#pragma once

#include <LibInterpolation/Spline/Interpolator.hh>

namespace Uni
{

class LinearSplineInterpolator : public SplineInterpolator
{
 public:
  [[nodiscard]] auto get_name() const -> std::string override
  {
    return std::string{"Linear spline (1,0)"};
  }

  [[nodiscard]] auto operator()(std::vector<Point<f64>> const& points)
      -> Spline override;
};

}  // namespace Uni
