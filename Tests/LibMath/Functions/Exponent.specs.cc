#include <gtest/gtest.h>

#include <LibMath/Constants.hh>
#include <LibMath/Math.hh>
#include <VS/Types.hh>

TEST(Math_Exponent, Example_zero)
{
  const f64 x = 0;

  const f64 expected = 1.0;
  const f64 actual = exp(x);

  EXPECT_EQ(expected, actual);
}

TEST(Math_Exponent, Example_half_1e6)
{
  const f64 x = 1.0 / 2;
  const f64 eps = 1e-6;

  const f64 expected = 1.648721270700128146848650787814163571L;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_Exponent, Example_half_1e12)
{
  const f64 x = 1.0 / 2;
  const f64 eps = 1e-12;

  const f64 expected = 1.648721270700128146848650787814163571L;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_Exponent, Example_one_1e6)
{
  const f64 x = 1.0;
  const f64 eps = 1e-6;

  const f64 expected = Constants::e;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_Exponent, Example_one_1e12)
{
  const f64 x = 1.0;
  const f64 eps = 1e-12;

  const f64 expected = Constants::e;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_Exponent, Example_minus_one_1e6)
{
  const f64 x = -1.0;
  const f64 eps = 1e-6;

  const f64 expected = Constants::inv_e;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_Exponent, Example_minus_one_1e12)
{
  const f64 x = -1.0;
  const f64 eps = 1e-12;

  const f64 expected = Constants::inv_e;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}

TEST(Math_Exponent, Example_minus_one_1e15)
{
  const f64 x = -1.0;
  const f64 eps = 1e-15;

  const f64 expected = Constants::inv_e;
  const f64 actual = exp(x, eps);

  EXPECT_NEAR(expected, actual, eps);
}
