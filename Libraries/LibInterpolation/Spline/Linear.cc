#include <LibInterpolation/Spline/Linear.hh>
#include <LibMath/Interval.hh>
#include <LibMath/Matrix.hh>
#include <LibSLE/GaussSolver.hh>
#include <cassert>
#include <iostream>
#include <memory>

namespace Uni
{

auto LinearSplineInterpolator::operator()(std::vector<Point> const& points)
    -> Spline
{
  assert(points.size() > 1);

  const auto N = points.size() - 1;

  std::vector<Spline::IntervalPolynomial> polynomials;
  polynomials.reserve(N);

  Matrix X = Matrix::zero(2 * N);
  Matrix y = Matrix::zero(2 * N, 1);

  for (std::size_t block = 0; block < N; ++block)
  {
    const auto index = 2 * block;
    X.at(index, index) = points.at(block).x;
    X.at(index + 1, index) = points.at(block + 1).x;

    X.at(index, index + 1) = 1.0;
    X.at(index + 1, index + 1) = 1.0;

    y.at(index) = points.at(block).y;
    y.at(index + 1) = points.at(block + 1).y;
  }

  auto solver = GaussSolver{};
  auto a = solver.solve(X, y);

  for (std::size_t i = 0; i < N; ++i)
  {
    const auto interval = Interval{points.at(i).x, points.at(i + 1).x};
    const auto polynomial = Polynomial{a.at(2 * i + 1), a.at(2 * i)};
    polynomials.emplace_back(interval, polynomial);
  }

  return {polynomials};
}

}  // namespace Uni
