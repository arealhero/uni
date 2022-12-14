#pragma once

#include <LibMath/Matrix.hh>
#include <string>

namespace Uni
{

class Solver
{
 public:
  constexpr static double DEFAULT_EPS = 1e-6;

  virtual ~Solver() = default;

  [[nodiscard]] virtual auto get_name() const -> std::string = 0;
  [[nodiscard]] virtual constexpr auto solve(const Matrix& A,
                                             const Matrix& b,
                                             double eps = DEFAULT_EPS) const
      -> Matrix = 0;
};

}  // namespace Uni
