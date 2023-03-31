#include <gtest/gtest.h>

#include <LibMath/Constants.hh>
#include <LibMath/Math.hh>
#include <VS/Types.hh>
#include <array>
#include <utility>

using namespace Constants;

TEST(Math_Sine, Example_pi_over_4_values_1e6)
{
  const f64 eps = 1e-6;
  const std::array values = {
      std::pair{0. * pi_over_4, 0.},
      std::pair{1. * pi_over_4, sqrt_2 / 2.},
      std::pair{2. * pi_over_4, 1.},
      std::pair{3. * pi_over_4, sqrt_2 / 2.},
      std::pair{4. * pi_over_4, 0.},
      std::pair{5. * pi_over_4, -sqrt_2 / 2.},
      std::pair{6. * pi_over_4, -1.},
      std::pair{7. * pi_over_4, -sqrt_2 / 2.},
      std::pair{8. * pi_over_4, 0.},
  };

  for (auto&& [x, expected] : values)
  {
    const f64 actual = sin(x, eps);
    EXPECT_NEAR(expected, actual, eps);
  }
}

TEST(Math_Sine, Example_pi_over_4_values_1e12)
{
  const f64 eps = 1e-12;
  const std::array values = {
      std::pair{0. * pi_over_4, 0.},
      std::pair{1. * pi_over_4, sqrt_2 / 2.},
      std::pair{2. * pi_over_4, 1.},
      std::pair{3. * pi_over_4, sqrt_2 / 2.},
      std::pair{4. * pi_over_4, 0.},
      std::pair{5. * pi_over_4, -sqrt_2 / 2.},
      std::pair{6. * pi_over_4, -1.},
      std::pair{7. * pi_over_4, -sqrt_2 / 2.},
      std::pair{8. * pi_over_4, 0.},
  };

  for (auto&& [x, expected] : values)
  {
    const f64 actual = sin(x, eps);
    EXPECT_NEAR(expected, actual, eps);
  }
}

TEST(Math_Sine, Example_pi_over_4_values_1e15)
{
  const f64 eps = 1e-15;
  const std::array values = {
      std::pair{0. * pi_over_4, 0.},
      std::pair{1. * pi_over_4, sqrt_2 / 2.},
      std::pair{2. * pi_over_4, 1.},
      std::pair{3. * pi_over_4, sqrt_2 / 2.},
      std::pair{4. * pi_over_4, 0.},
      std::pair{5. * pi_over_4, -sqrt_2 / 2.},
      std::pair{6. * pi_over_4, -1.},
      std::pair{7. * pi_over_4, -sqrt_2 / 2.},
      std::pair{8. * pi_over_4, 0.},
  };

  for (auto&& [x, expected] : values)
  {
    const f64 actual = sin(x, eps);
    EXPECT_NEAR(expected, actual, eps);
  }
}

TEST(Math_Sine, Example_pi_over_6_values_1e6)
{
  const f64 eps = 1e-6;
  const std::array values = {
      std::pair{0.  * pi / 6., 0.},
      std::pair{1.  * pi / 6., 1. / 2.},
      std::pair{2.  * pi / 6., sqrt_3 / 2.},
      std::pair{3.  * pi / 6., 1.},
      std::pair{4.  * pi / 6., sqrt_3 / 2.},
      std::pair{5.  * pi / 6., 1. / 2.},
      std::pair{6.  * pi / 6., 0.},
      std::pair{7.  * pi / 6., -1. / 2.},
      std::pair{8.  * pi / 6., -sqrt_3 / 2.},
      std::pair{9.  * pi / 6., -1.},
      std::pair{10. * pi / 6., -sqrt_3 / 2.},
      std::pair{11. * pi / 6., -1. / 2.},
      std::pair{12. * pi / 6., 0.},
  };

  for (auto&& [x, expected] : values)
  {
    const f64 actual = sin(x, eps);
    EXPECT_NEAR(expected, actual, eps);
  }
}

TEST(Math_Sine, Example_pi_over_6_values_1e12)
{
  const f64 eps = 1e-12;
  const std::array values = {
      std::pair{0.  * pi / 6., 0.},
      std::pair{1.  * pi / 6., 1. / 2.},
      std::pair{2.  * pi / 6., sqrt_3 / 2.},
      std::pair{3.  * pi / 6., 1.},
      std::pair{4.  * pi / 6., sqrt_3 / 2.},
      std::pair{5.  * pi / 6., 1. / 2.},
      std::pair{6.  * pi / 6., 0.},
      std::pair{7.  * pi / 6., -1. / 2.},
      std::pair{8.  * pi / 6., -sqrt_3 / 2.},
      std::pair{9.  * pi / 6., -1.},
      std::pair{10. * pi / 6., -sqrt_3 / 2.},
      std::pair{11. * pi / 6., -1. / 2.},
      std::pair{12. * pi / 6., 0.},
  };

  for (auto&& [x, expected] : values)
  {
    const f64 actual = sin(x, eps);
    EXPECT_NEAR(expected, actual, eps);
  }
}
