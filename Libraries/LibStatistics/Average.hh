#pragma once

#include <numeric>
#include <vector>

namespace Uni
{

constexpr double arithmetic_average(const std::vector<double>& numbers)
{
  return std::accumulate(numbers.begin(), numbers.end(), 0.0) / numbers.size();
}

}  // namespace Uni
