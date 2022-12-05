#include <LibApproximation/LeastSquaresApproximator.hh>
#include <LibStatistics/Average.hh>
#include <cassert>
#include <stdexcept>
#include <unordered_map>

namespace Uni
{

Polynomial LeastSquaresApproximator::operator()(
    const std::vector<Point>& points,
    const std::size_t degree) const
{
  return (this->*m_method)(points, degree);
}

Polynomial LeastSquaresApproximator::orthogonal_polynomials_method(
    const std::vector<Point>& points,
    const std::size_t degree) const
{
  const std::size_t M = points.size();
  const std::size_t N = degree;

  // Orthogonal polynomials generation
  std::vector<double> xs(M);
  std::transform(points.begin(),
                 points.end(),
                 xs.begin(),
                 [](const Point& point) -> double { return point.x; });

  std::vector<Polynomial> q = {Polynomial{1.0},
                               Polynomial{-arithmetic_average(xs), 1.0}};
  q.reserve(N + 1);

  const auto generate_alpha = [&xs, &q](const std::size_t index) -> double
  {
    const auto prev = q.at(index - 1);
    double dividend = 0.0, divider = 0.0;
    for (const auto& x : xs)
    {
      const auto value = prev(x);
      const auto square = value * value;
      dividend += x * square;
      divider += square;
    }
    return dividend / divider;
  };
  const auto generate_beta = [&xs, &q](const std::size_t index) -> double
  {
    const auto prev = q.at(index - 1), current = q.at(index);
    double dividend = 0.0, divider = 0.0;
    for (const auto& x : xs)
    {
      const auto prev_value = prev(x), current_value = current(x);
      dividend += x * prev_value * current_value;
      divider += prev_value * prev_value;
    }
    return dividend / divider;
  };

  for (std::size_t i = 2; i < N + 1; ++i)
  {
    const auto prev = q.at(i - 1);
    const auto pprev = q.at(i - 2);
    const auto alpha = generate_alpha(i);
    const auto beta = generate_beta(i - 1);

    q.emplace_back((Polynomial{0.0, 1.0} - alpha) * prev - beta * pprev);
  }

  auto result = Polynomial{0.0};

  const auto generate_a = [&q, &points](const std::size_t index) -> double
  {
    const auto current = q.at(index);
    double dividend = 0.0, divider = 0.0;
    for (const auto& [x, y] : points)
    {
      const auto current_value = current(x);
      dividend += current_value * y;
      divider += current_value * current_value;
    }
    return dividend / divider;
  };

  for (std::size_t i = 0; i < N + 1; ++i)
  {
    const auto a = generate_a(i);
    result += a * q.at(i);
  }

  return result;
}

Polynomial LeastSquaresApproximator::normal_equations_method(
    const std::vector<Point>& points,
    const std::size_t degree) const
{
  const std::size_t M = points.size();
  const std::size_t N = degree;

  auto E = Matrix(M, N + 1);
  auto f = Matrix(M, 1);
  for (std::size_t row = 0; row < M; ++row)
  {
    const auto [x, y] = points.at(row);

    double factor = 1.0;
    for (std::size_t col = 0; col < N + 1; ++col)
    {
      E.at(row, col) = factor;
      factor *= x;
    }
    f.at(row) = y;
  }

  const auto E_t = E.T();
  const auto solver = GaussSolver{};
  const auto a = solver.solve(E_t * E, E_t * f);

  assert(a.rows() == N + 1 && a.cols() == 1);
  return Polynomial(a.get_data());
}

}  // namespace Uni
