#pragma once

#include <LibApproximation/Approximator.hh>
#include <LibMath/Matrix.hh>
#include <LibSLE/GaussSolver.hh>
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

  std::string get_name() const override { return {"Least squares"}; }

  Polynomial operator()(const std::vector<Point>& points,
                        const std::size_t degree) const override;

 private:
  using Method = Polynomial (LeastSquaresApproximator::*)(
      const std::vector<Point>& points,
      const std::size_t degree) const;
  Method m_method;

  Polynomial orthogonal_polynomials_method(const std::vector<Point>& points,
                                           const std::size_t degree) const;
  Polynomial normal_equations_method(const std::vector<Point>& points,
                                     const std::size_t degree) const;
};

}  // namespace Uni
