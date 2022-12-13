#include <LibInterpolation/LinearGenerator.hh>
#include <LibInterpolation/OptimalGenerator.hh>
#include <LibInterpolation/Polynomial/Lagrange.hh>
#include <LibInterpolation/Polynomial/Newton.hh>
#include <LibInterpolation/Spline/Cubic.hh>
#include <LibInterpolation/Spline/Linear.hh>
#include <LibInterpolation/Spline/Quadratic.hh>
#include <LibMath/Point.hh>
#include <LibMath/Polynomial.hh>
#include <cmath>
#include <iomanip>
#include <iostream>

auto f(double x) -> double
{
  return x + std::log10(x / 5);
}

constexpr std::size_t M = 10000;

auto calc_max(const Uni::Function& function, double a, double b) -> double
{
  const double step = (b - a) / (M - 1);
  double max_diff = 0;
  for (std::uint32_t i = 0; i < M; ++i)
  {
    const double x = a + i * step;
    const double diff = std::abs(f(x) - function(x));
    if (diff > max_diff)
    {
      max_diff = diff;
    }
  }
  return max_diff;
}

// FIXME: rename
template <typename T>
auto print_table(std::shared_ptr<Uni::Interpolator<T>> interpolator,
                 double a,
                 double b) -> void
{
  std::cout << "=== " << interpolator->get_name() << " ===\n";
  std::cout << std::setw(10) << 'N' << std::setw(10) << 'M' << std::setw(20)
            << "R" << std::setw(20) << "R opt" << std::setw(20) << '\n';
  for (auto n : {3, 5, 7, 10, 15, 20, 25, 30, 50, 75, 100, 200, 500, 1000})
  {
    auto linear_generator =
        std::make_shared<Uni::LinearGenerator<double>>(a, b, n);
    auto optimal_generator =
        std::make_shared<Uni::OptimalGenerator<double>>(a, b, n);

    constexpr double EPS = 1e-12;

    std::vector<Uni::Point> linear_points;
    linear_points.reserve(n);
    for (auto x : *linear_generator)
    {
      linear_points.push_back({x, f(x)});
    }

    std::vector<Uni::Point> optimal_points;
    optimal_points.reserve(n);
    for (auto x : *optimal_generator)
    {
      optimal_points.push_back({x, f(x)});
    }

    auto linear_polynomial = (*interpolator)(linear_points);
    auto linear_max = calc_max(
        linear_polynomial, linear_points.front().x, linear_points.back().x);

    auto optimal_polynomial = (*interpolator)(optimal_points);
    auto optimal_max = calc_max(optimal_polynomial,
                                optimal_points.front().x + EPS,
                                optimal_points.back().x - EPS);

    std::cout << std::fixed << std::setw(10) << n << std::setw(10) << M
              << std::setw(20) << std::setprecision(7) << std::scientific
              << linear_max << std::setw(20) << optimal_max << std::setw(20)
              << '\n';
  }
}

auto main() -> int
{
  const double start = 0.1;
  const double end = 1.0;

  auto lagrange_interpolator = std::make_shared<Uni::LagrangeInterpolator>();
  auto newton_interpolator = std::make_shared<Uni::NewtonInterpolator>();
  print_table<Uni::Polynomial>(lagrange_interpolator, start, end);
  print_table<Uni::Polynomial>(newton_interpolator, start, end);

  auto linear_spline_interpolator =
      std::make_shared<Uni::LinearSplineInterpolator>();
  auto quadratic_spline_interpolator =
      std::make_shared<Uni::QuadraticSplineInterpolator>();
  auto cubic_spline_interpolator =
      std::make_shared<Uni::CubicSplineInterpolator>();
  print_table<Uni::Spline>(linear_spline_interpolator, start, end);
  print_table<Uni::Spline>(quadratic_spline_interpolator, start, end);
  print_table<Uni::Spline>(cubic_spline_interpolator, start, end);

  return 0;
}
