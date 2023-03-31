#include <gtest/gtest.h>

#include <LibMath/Algebra.hh>
#include <algorithm>

using namespace LibMath::Numerical::Algebra;

using Roots = std::tuple<f64, f64, f64>;

constexpr void TuplesAreEqual(const Roots& expected, const Roots& actual)
{
  std::vector expected_vec = {
      std::get<0>(expected), std::get<1>(expected), std::get<2>(expected)};
  std::vector actual_vec = {
      std::get<0>(actual), std::get<1>(actual), std::get<2>(actual)};

  std::sort(expected_vec.begin(), expected_vec.end());
  std::sort(actual_vec.begin(), actual_vec.end());

  for (std::size_t i = 0; i < 3; ++i)
  {
    std::cout << actual_vec.at(i) << '\n';
    EXPECT_EQ(expected_vec.at(i), actual_vec.at(i));
  }
}

TEST(Algebra_CubicEquation, Root_1)
{
  const auto expected = std::make_tuple(1., 2., 3.);
  const auto actual = find_roots(1., -6., 11., -6.);

  TuplesAreEqual(expected, actual);
}
