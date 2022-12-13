#include "Matrix.hh"

namespace Uni
{

std::ostream& operator<<(std::ostream& out, const Matrix& matrix)
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
