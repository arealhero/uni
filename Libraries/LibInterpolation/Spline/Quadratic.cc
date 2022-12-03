#include <LibInterpolation/Spline/Quadratic.hh>
#include <LibMath/Interval.hh>
#include <LibMath/Matrix.hh>
#include <LibSLE/GaussSolver.hh>
#include <cassert>
#include <iostream>
#include <memory>

namespace Uni
{

Spline QuadraticSplineInterpolator::operator()(
    std::vector<Point> const& points)
{
  assert(points.size() > 1);

  const auto N = points.size() - 1;
  const double d = 0.0;

  std::vector<Spline::IntervalPolynomial> polynomials;
  polynomials.reserve(N);

  Matrix X = Matrix::zero(3 * N);
  Matrix y = Matrix::zero(3 * N, 1);

  for (std::size_t block = 0; block < N; ++block)
  {
    const auto row = 2 * block;
    const auto col = 3 * block;

    const auto [left_x, left_y] = points.at(block);
    const auto [right_x, right_y] = points.at(block + 1);

    X.at(row, col) = left_x * left_x;
    X.at(row, col + 1) = left_x;
    X.at(row, col + 2) = 1.0;
    y.at(row) = left_y;

    X.at(row + 1, col) = right_x * right_x;
    X.at(row + 1, col + 1) = right_x;
    X.at(row + 1, col + 2) = 1.0;
    y.at(row + 1) = right_y;
  }

  for (std::size_t block = 0; block < N - 1; ++block)
  {
    const auto row = 2 * N + block;
    const auto col = 3 * block;

    const auto x = points.at(block + 1).x;

    X.at(row, col) = 2.0 * x;
    X.at(row, col + 1) = 1.0;

    X.at(row, col + 3) = -2.0 * x;
    X.at(row, col + 4) = -1.0;
  }

  X.at(3 * N - 1, 3 * N - 3) = 2.0 * points.at(N).x;
  X.at(3 * N - 1, 3 * N - 2) = 1.0;
  y.at(3 * N - 1) = d;

  auto solver = GaussSolver{};
  auto a = solver.solve(X, y);

  for (std::size_t i = 0; i < N; ++i)
  {
    const auto index = 3 * i;
    const auto interval = Interval{points.at(i).x, points.at(i + 1).x};
    const auto polynomial =
        Polynomial{a.at(index + 2), a.at(index + 1), a.at(index)};
    polynomials.push_back({interval, polynomial});
  }

  return Spline(polynomials);
}

}  // namespace Uni
