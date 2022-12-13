#include "Matrix.hh"

namespace Uni
{

auto operator<<(std::ostream& out, const Matrix& matrix) -> std::ostream&
{
  for (std::size_t i = 0; i < matrix.rows(); ++i)
  {
    for (std::size_t j = 0; j < matrix.cols(); ++j)
    {
      out << matrix.at(i, j) << ' ';
    }
    out << '\n';
  }
  return out;
}

}  // namespace Uni
