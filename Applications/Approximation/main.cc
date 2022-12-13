#include <LibApproximation/LeastSquares.hh>
#include <LibGUI/Plot.hh>
#include <LibInterpolation/LinearGenerator.hh>
#include <LibMath/Point.hh>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

constexpr const double A = -1.0;
constexpr const double B = 1.0;

constexpr const std::size_t NUMBER_OF_UNIQUE_XS = 100;
constexpr const std::size_t NUMBER_OF_APPROXIMATIONS = 3;
constexpr const double APPROXIMATION_ERROR = 0.1;

double f(const double x)
{
  return x * std::sqrt(x + 2.0);
}

double get_random_error()
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dis(-APPROXIMATION_ERROR,
                                              APPROXIMATION_ERROR);
  return dis(gen);
}

std::vector<double> generate_approximations(const double x)
{
  std::vector<double> approximations(NUMBER_OF_APPROXIMATIONS);
  for (auto& approximation : approximations)
  {
    approximation = f(x) + get_random_error();
  }
  return approximations;
}

double calculate_total_error(const Uni::Polynomial& polynomial,
                             const std::vector<Uni::Point>& points)
{
  double total_error = 0;
  for (const auto& [x, y] : points)
  {
    const double error = std::abs(polynomial(x) - y);
    total_error += error * error;
  }
  return total_error;
}

void print_table(const std::vector<Uni::Point>& points)
{
  const auto normal_equations_approximator = Uni::LeastSquaresApproximator{
      Uni::LeastSquaresApproximator::GeneratorType::NormalEquations};
  const auto orthogonal_polynomials_approximator =
      Uni::LeastSquaresApproximator{
          Uni::LeastSquaresApproximator::GeneratorType::OrthogonalPolynomials};

  for (std::size_t degree = 0; degree < NUMBER_OF_UNIQUE_XS; ++degree)
  {
    const auto normal_equations_polynomial =
        normal_equations_approximator(points, degree);
    const auto orthogonal_polynomials_polynomial =
        orthogonal_polynomials_approximator(points, degree);

    const auto normal_equations_error =
        calculate_total_error(normal_equations_polynomial, points);
    const auto orthogonal_polynomials_error =
        calculate_total_error(orthogonal_polynomials_polynomial, points);

    std::cout << std::fixed << std::setw(4) << degree << std::scientific
              << std::setprecision(6) << std::setw(20)
              << normal_equations_error << std::setprecision(6)
              << std::setw(20) << orthogonal_polynomials_error << '\n';

    if (degree <= 5)
    {
      std::stringstream ss;
      ss << "p(" << degree << ")";

      Uni::Gui::subplot(3, 3, degree + 1);
      Uni::Gui::named_plot(ss.str(), normal_equations_polynomial, A, B);
      Uni::Gui::subplot(3, 3, degree + 1);
      Uni::Gui::plot(points);
      Uni::Gui::legend();
    }
  }
}

int main()
{
  std::vector<Uni::Point> points;
  points.reserve(NUMBER_OF_UNIQUE_XS * NUMBER_OF_APPROXIMATIONS);

  auto generator = Uni::LinearGenerator<double>(A, B, NUMBER_OF_UNIQUE_XS);
  for (const double x : generator)
  {
    const auto approximations = generate_approximations(x);
    for (const double approximation : approximations)
    {
      points.push_back({x, approximation});
    }
  }

  Uni::Gui::plot_init();

  try
  {
    print_table(points);
    Uni::Gui::show_plot();
  }
  catch (const std::runtime_error& e)
  {
    std::cout << "Caught exception: " << e.what() << '\n';
  }
}
