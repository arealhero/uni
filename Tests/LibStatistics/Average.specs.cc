#include <gtest/gtest.h>

#include <LibStatistics/Numbers.hh>
#include <VS/Vector.hh>
#include <vector>

TEST(Average, SimpleExample)
{
  const VS::Vector<double> points{
      1.0,
      2.0,
      3.0,
      4.0,
      5.0,
  };
  const auto average = Uni::mean(points);
  ASSERT_DOUBLE_EQ(average, 3.0);
}
