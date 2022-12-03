#include <gtest/gtest.h>

#include <LibInterpolation/Spline/Linear.hh>
#include <LibMath/Point.hh>
#include <vector>

TEST(LinearSplineInterpolator, SimpleExample)
{
  const std::vector<Uni::Point> points{
      {0.0, 1.0},
      {2.0, 3.0},
      {3.0, 2.0},
  };

  auto interpolator = Uni::LinearSplineInterpolator{};
  auto spline = interpolator(points);

  const auto& polynomials = spline.get_polynomials();

  EXPECT_EQ(polynomials.at(0).second, Uni::Polynomial({1.0, 1.0}));
  EXPECT_EQ(polynomials.at(1).second, Uni::Polynomial({5.0, -1.0}));
}
