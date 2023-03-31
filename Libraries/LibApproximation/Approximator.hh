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

  [[nodiscard]] virtual auto get_name() const -> std::string = 0;
  [[nodiscard]] virtual auto operator()(const std::vector<Point<f64>>& points,
                                        std::size_t degree) const
      -> Polynomial = 0;
};

}  // namespace Uni
