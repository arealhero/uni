#include <LibInterpolation/Polynomial/Lagrange.hh>
#include <LibInterpolation/LinearGenerator.hh>
#include <LibInterpolation/Polynomial/Newton.hh>
#include <LibInterpolation/OptimalGenerator.hh>
#include <LibMath/Point.hh>
#include <LibMath/Polynomial.hh>
#include <cmath>
#include <iomanip>
#include <iostream>

double f(double x)
{
  return x + std::log10(x / 5);
}

constexpr std::size_t M = 3000;

double calc_max(const Uni::Polynomial& polynomial, double a, double b)
{
  const double step = (b - a) / (M - 1);
  double max_diff = 0;
  for (std::size_t i = 0; i < M; ++i)
  {
    const double x = a + i * step;
    const double diff = std::abs(f(x) - polynomial(x));
    if (diff > max_diff)
      max_diff = diff;
  }
  return max_diff;
}

// FIXME: rename
void print_table(std::shared_ptr<Uni::PolynomialInterpolator> interpolation,
                 double a,
                 double b)
{
  std::cout << "=== " << interpolation->get_name() << " ===\n";
  for (auto n : {2, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50})
  {
    auto linear_generator =
        std::make_shared<Uni::LinearGenerator<double>>(a, b, n);
    auto optimal_generator =
        std::make_shared<Uni::OptimalGeneartor<double>>(a, b, n);

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

    auto linear_polynomial = (*interpolation)(linear_points);
    auto linear_max = calc_max(linear_polynomial, a, b);

    auto optimal_polynomial = (*interpolation)(optimal_points);
    auto optimal_max = calc_max(optimal_polynomial, a, b);

    std::cout << std::setw(10) << n << std::setw(10) << M << std::setw(20)
              << linear_max << std::setw(20) << optimal_max << std::setw(20)
              << '\n';
  }
}

int main()
{
  const double start = 0.1;
  const double end = 1.0;

  auto lagrange_interpolation = std::make_shared<Uni::LagrangeInterpolator>();
  auto newton_interpolation = std::make_shared<Uni::NewtonInterpolator>();
  print_table(lagrange_interpolation, start, end);
  print_table(newton_interpolation, start, end);

  // spline 1,0
  // spline 2,1
  // spline 3,2

  return 0;
}
