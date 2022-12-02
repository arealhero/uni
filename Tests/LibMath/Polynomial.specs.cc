#include <gtest/gtest.h>

#include <LibMath/Polynomial.hh>

TEST(Polynomial, Differentiation)
{
  const Uni::Polynomial poly{{1.0, 2.0, 3.0}};

  auto d = poly.d();
  EXPECT_EQ(d, Uni::Polynomial({2.0, 6.0}));

  d = d.d();
  EXPECT_EQ(d, Uni::Polynomial({6.0}));

  d = d.d();
  EXPECT_EQ(d, Uni::Polynomial({0.0}));

  d = d.d();
  EXPECT_EQ(d, Uni::Polynomial({0.0}));
}
