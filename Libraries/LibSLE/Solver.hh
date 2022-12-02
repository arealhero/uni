#pragma once

#include <LibMath/Matrix.hh>
#include <string>

namespace Uni
{

class Solver
{
 public:
  virtual ~Solver() = default;

  [[nodiscard]] virtual std::string get_name() const = 0;
  [[nodiscard]] virtual constexpr Matrix solve(const Matrix& A,
                                               const Matrix& b,
                                               const double eps) = 0;
};

}  // namespace Uni
