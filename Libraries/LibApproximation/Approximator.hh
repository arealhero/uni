#pragma once

#include <LibMath/Point.hh>
#include <LibMath/Polynomial.hh>
#include <string>
#include <vector>

namespace Uni
{

class Approximator
{
 public:
  virtual ~Approximator() = default;

  virtual std::string get_name() const = 0;
  virtual Polynomial operator()(const std::vector<Point>& points,
                                const std::size_t degree) const = 0;
};

}  // namespace Uni
