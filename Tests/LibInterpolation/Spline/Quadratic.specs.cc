#include <gtest/gtest.h>

#include <LibInterpolation/Spline/Quadratic.hh>
#include <LibMath/Point.hh>
#include <vector>

// FIXME: implement in Polynomial::operator==
void assert_equal(const Uni::Polynomial& lhs, const Uni::Polynomial& rhs)
{
  ASSERT_EQ(lhs.degree(), rhs.degree());
  for (std::size_t i = 0; i < lhs.degree(); ++i)
  {
    ASSERT_DOUBLE_EQ(lhs.coeff(i), rhs.coeff(i));
  }
}

TEST(QuadraticSplineInterpolator, SimpleExample)
{
  const std::vector<Uni::Point> points{
      {0.0, 1.0},
      {2.0, 3.0},
      {3.0, 2.0},
  };

  auto interpolator = Uni::QuadraticSplineInterpolator{};
  auto spline = interpolator(points);

  const auto& polynomials = spline.get_polynomials();

  assert_equal(polynomials.at(0).second,
                      Uni::Polynomial({1.0, 4.0, -1.5}));
  assert_equal(polynomials.at(1).second,
                      Uni::Polynomial({11.0, -6.0, 1.0}));
}
