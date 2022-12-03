#include <gtest/gtest.h>

#include <LibInterpolation/Spline/Cubic.hh>
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

TEST(CubicSplineInterpolator, SimpleExample)
{
  const std::vector<Uni::Point> points{
      {0.0, 1.0},
      {0.25, 2.0},
      {0.5, 1.0},
      {0.75, 0.0},
      {1.0, 1.0},
  };

  auto interpolator = Uni::CubicSplineInterpolator{};
  auto spline = interpolator(points);

  const auto& polynomials = spline.get_polynomials();

  assert_equal(polynomials.at(0).second,
               Uni::Polynomial({1.0, 6.0, 0.0, -32.0}));
  assert_equal(polynomials.at(1).second,
               Uni::Polynomial({0.0, 18.0, -48.0, 32.0}));
  assert_equal(polynomials.at(2).second,
               Uni::Polynomial({0.0, 18.0, -48.0, 32.0}));
  assert_equal(polynomials.at(3).second,
               Uni::Polynomial({27.0, -90.0, 96.0, -32.0}));
}
