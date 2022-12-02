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

  virtual std::string get_name() const = 0;
  virtual InterpolationType operator()(std::vector<Point> const& points) = 0;
};

}  // namespace Uni
