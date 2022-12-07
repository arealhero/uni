#include <gtest/gtest.h>

#include <LibApproximation/LeastSquaresApproximator.hh>
#include <LibMath/Point.hh>
#include <vector>

// FIXME: implement in Polynomial::operator==
inline void assert_equal(const Uni::Polynomial& lhs,
                         const Uni::Polynomial& rhs,
                         const double error = 1e-12)
{
  ASSERT_EQ(lhs.degree(), rhs.degree());
  for (std::size_t i = 0; i < lhs.degree(); ++i)
  {
    ASSERT_NEAR(lhs.coeff(i), rhs.coeff(i), error);
  }
}

TEST(LeastSquaresApproximator, NormalEquations_SimpleExample)
{
  const std::vector<Uni::Point> points = {
      {1.0, 1.0},
      {2.0, -1.0},
      {3.0, 1.0},
  };

  const auto type =
      Uni::LeastSquaresApproximator::GeneratorType::NormalEquations;
  const auto approximator = Uni::LeastSquaresApproximator{type};
  const auto constant = approximator(points, 0);
  const auto linear = approximator(points, 1);
  const auto quadratic = approximator(points, 2);

  assert_equal(constant, Uni::Polynomial{1.0 / 3.0});
  assert_equal(linear, Uni::Polynomial{1.0 / 3.0});
  assert_equal(quadratic, Uni::Polynomial{7.0, -8.0, 2.0});
}

TEST(LeastSquaresApproximator, OrthogonalPolynomials_SimpleExample)
{
  const std::vector<Uni::Point> points = {
      {1.0, 1.0},
      {2.0, -1.0},
      {3.0, 1.0},
  };

  const auto type =
      Uni::LeastSquaresApproximator::GeneratorType::OrthogonalPolynomials;
  const auto approximator = Uni::LeastSquaresApproximator{type};
  const auto constant = approximator(points, 0);
  const auto linear = approximator(points, 1);
  const auto quadratic = approximator(points, 2);

  assert_equal(constant, Uni::Polynomial{1.0 / 3.0});
  assert_equal(linear, Uni::Polynomial{1.0 / 3.0});
  assert_equal(quadratic, Uni::Polynomial{7.0, -8.0, 2.0});
}
