#pragma once

#include <LibMath/Point.hh>
#include <string>
#include <vector>

namespace Uni
{

template <typename InterpolationType>
class Interpolator
{
 public:
  virtual ~Interpolator() = default;

  [[nodiscard]] virtual auto get_name() const -> std::string = 0;
  [[nodiscard]] virtual auto operator()(std::vector<Point> const& points)
      -> InterpolationType = 0;
};

}  // namespace Uni
