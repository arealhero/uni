#include <gtest/gtest.h>

#include <LibStatistics/Average.hh>
#include <vector>

TEST(Average, SimpleExample)
{
  const std::vector<double> points{
      1.0,
      2.0,
      3.0,
      4.0,
      5.0,
  };
  const auto average = Uni::arithmetic_average(points);
  ASSERT_DOUBLE_EQ(average, 3.0);
}
