#include <LibInterpolation/Spline/Cubic.hh>
#include <LibMath/Interval.hh>
#include <LibMath/Matrix.hh>
#include <LibSLE/GaussSolver.hh>
#include <cassert>
#include <iostream>
#include <memory>

namespace Uni
{

Spline CubicSplineInterpolator::operator()(std::vector<Point> const& points)
{
  assert(points.size() > 1);

  const auto N = points.size() - 1;

  std::vector<Spline::IntervalPolynomial> polynomials;
  polynomials.reserve(N);

  auto H = Matrix::zero(N - 1);
  auto gamma = Matrix::zero(N - 1, 1);
  for (std::size_t i = 0; i < N - 1; ++i)
  {
    const auto [x_prev, y_prev] = points.at(i);
    const auto [x_current, y_current] = points.at(i + 1);
    const auto [x_next, y_next] = points.at(i + 2);

    const auto h_prev = x_current - x_prev;
    const auto h_next = x_next - x_current;

    if (i != 0)
      H.at(i, i - 1) = h_prev;
    H.at(i, i) = 2 * (h_prev + h_next);
    if (i != N - 2)
      H.at(i, i + 1) = h_next;

    gamma.at(i) =
        6 * ((y_next - y_current) / h_next - (y_current - y_prev) / h_prev);
  }

  auto solver = GaussSolver{};
  auto ypp_partial = solver.solve(H, gamma);

  auto ypp = Matrix::zero(N + 1, 1);
  for (std::size_t i = 0; i < N - 1; ++i)
  {
    ypp.at(i + 1) = ypp_partial.at(i);
  }

  auto yp = Matrix::zero(N, 1);
  for (std::size_t i = 0; i < N; ++i)
  {
    const auto [x, y] = points.at(i);
    const auto [x_next, y_next] = points.at(i + 1);
    const auto h = x_next - x;
    yp.at(i) =
        (y_next - y) / h - ypp.at(i + 1) * h / 6.0 - ypp.at(i) * h / 3.0;
  }

  for (std::size_t i = 0; i < N; ++i)
  {
    const auto [x_i, y_i] = points.at(i);
    const auto x_next = points.at(i + 1).x;
    const auto interval = Interval{x_i, x_next};

    const auto term = Polynomial{-x_i, 1.0};
    // clang-format off
    const auto polynomial = Polynomial{y_i}
                            + yp.at(i) * term
                            + ypp.at(i) / 2.0 * term * term
                            + (ypp.at(i + 1) - ypp.at(i)) / (6 * (x_next - x_i))
                              * term * term * term;
    // clang-format on

    polynomials.push_back({interval, polynomial});
  }

  return Spline(polynomials);
}

}  // namespace Uni
