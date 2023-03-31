#include <gtest/gtest.h>

#include <LibMath/Constants.hh>
#include <LibMath/Math.hh>
#include <VS/Types.hh>

TEST(Math_SquareRoot, Example_zero)
{
  const f64 x = 0;

  const f64 expected = 0.0;
  const f64 actual = sqrt(x);

  EXPECT_EQ(expected, actual);
}

TEST(Math_SquareRoot, Example_one)
{
  const f64 x = 1.0;

  const f64 expected = 1.0;
  const f64 actual = sqrt(x);

  EXPECT_EQ(expected, actual);
}

TEST(Math_SquareRoot, Example_two_1e6)
{
  const f64 x = 2.0;
  const f64 eps = 1e-6;

  const f64 expected = Constants::sqrt_2;
  const f64 actual = sqrt(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_SquareRoot, Example_two_1e12)
{
  const f64 x = 2.0;
  const f64 eps = 1e-12;

  const f64 expected = Constants::sqrt_2;
  const f64 actual = sqrt(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_SquareRoot, Example_four_1e6)
{
  const f64 x = 4.0;
  const f64 eps = 1e-6;

  const f64 expected = 2.0;
  const f64 actual = sqrt(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_SquareRoot, Example_four_1e12)
{
  const f64 x = 4.0;
  const f64 eps = 1e-12;

  const f64 expected = 2.0;
  const f64 actual = sqrt(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}
