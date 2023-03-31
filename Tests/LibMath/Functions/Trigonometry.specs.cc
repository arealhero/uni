#include <gtest/gtest.h>

#include <LibMath/Constants.hh>
#include <LibMath/Math.hh>
#include <VS/Types.hh>

using namespace Constants;

TEST(Math_Trigonometry, pythagorean_identity_1e6)
{
  const f64 eps = 1e-6;
  const f64 expected = 1.;
  const f64 step = 0.0001;

  for (f64 x = 0.; x <= two_pi; x += step)
  {
    const auto c = cos(x, eps);
    const auto s = sin(x, eps);
    const f64 actual = c * c + s * s;

    // FIXME: calculate an actual error
    EXPECT_NEAR(expected, actual, 4. * eps);
  }
}
