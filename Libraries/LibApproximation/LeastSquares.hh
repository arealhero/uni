#pragma once

#include <LibApproximation/Approximator.hh>
#include <LibMath/Matrix.hh>
#include <LibSLE/GaussSolver.hh>
#include <LibStatistics/Numbers.hh>
#include <algorithm>
#include <unordered_set>

namespace Uni
{

class LeastSquaresApproximator : public Approximator
{
 public:
  enum class GeneratorType
  {
    NormalEquations,
    OrthogonalPolynomials,
  };

  explicit LeastSquaresApproximator(const GeneratorType type)
  {
    switch (type)
    {
      case GeneratorType::NormalEquations:
        m_method = &LeastSquaresApproximator::normal_equations_method;
        break;
      case GeneratorType::OrthogonalPolynomials:
        m_method = &LeastSquaresApproximator::orthogonal_polynomials_method;
        break;
      default:
        throw std::runtime_error{"Unknown type"};
    }
  }

  [[nodiscard]] auto get_name() const -> std::string override
  {
    return {"Least squares"};
  }

  [[nodiscard]] constexpr auto operator()(const std::vector<Point>& points,
                                          const std::size_t degree) const
      -> Polynomial override
  {
    return (*m_method)(points, degree);
  }

 private:
  using Method = Polynomial (*)(const std::vector<Point>& points,
                                const std::size_t degree);
  Method m_method;

  [[nodiscard]] constexpr static auto orthogonal_polynomials_method(
      const std::vector<Point>& points,
      const std::size_t degree) -> Polynomial
  {
    const std::size_t M = points.size();
    const std::size_t N = degree;

    // Orthogonal polynomials generation
    VS::Vector<double> xs(M, 0.0);
    std::transform(points.begin(),
                   points.end(),
                   xs.begin(),
                   [](const Point& point) -> double { return point.x; });

    std::vector<Polynomial> q = {Polynomial{1.0}, Polynomial{-mean(xs), 1.0}};
    q.reserve(N + 1);

    const auto generate_alpha = [&xs, &q](const std::size_t index) -> double
    {
      const auto prev = q.at(index - 1);
      double dividend = 0.0;
      double divider = 0.0;
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
      const auto prev = q.at(index - 1);
      const auto current = q.at(index);
      double dividend = 0.0;
      double divider = 0.0;
      for (const auto& x : xs)
      {
        const auto prev_value = prev(x);
        const auto current_value = current(x);
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
      double dividend = 0.0;
      double divider = 0.0;
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

  [[nodiscard]] constexpr static auto normal_equations_method(
      const std::vector<Point>& points,
      const std::size_t degree) -> Polynomial
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
    return {a.get_data()};
  }
};

}  // namespace Uni
