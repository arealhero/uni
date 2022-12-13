#include <LibInterpolation/Polynomial/Newton.hh>

namespace Uni
{

Polynomial NewtonInterpolator::operator()(const std::vector<Point>& points)
{
  const std::size_t N = points.size();

  std::vector<double> differences;
  differences.reserve((1 + N) * N / 2);

  for (const auto& point : points)
  {
    differences.emplace_back(point.y);
  }

  std::size_t index = N;
  for (std::size_t d = 1; d < N; ++d)
  {
    for (std::size_t i = 0; i < N - d; ++i)
    {
      const std::size_t left = i;
      const std::size_t right = i + d;

      const double left_x = points[left].x;
      const double right_x = points[right].x;
      const double left_y = differences[index - N + d - 1];
      const double right_y = differences[index - N + d];

      differences.emplace_back((left_y - right_y) / (left_x - right_x));

      ++index;
    }
  }

  Polynomial result;
  index = 0;
  for (std::size_t k = 0; k < N; ++k)
  {
    Polynomial term(1.0);
    for (std::size_t i = 0; i < k; ++i)
    {
      const auto x_i = points.at(i).x;
      term *= Polynomial({-x_i, 1.0});
    }

    const auto factor = differences[index];
    result += factor * term;
    index += N - k;
  }

  return result;
}

}  // namespace Uni
