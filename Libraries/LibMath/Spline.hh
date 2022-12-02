#pragma once

#include "Polynomial.hh"
#include "Interval.hh"
#include <utility>
#include <vector>

namespace Uni
{

class Spline
{
 public:
  using IntervalPolynomial = std::pair<Interval, Polynomial>;

  Spline(const std::initializer_list<IntervalPolynomial>& intervals)
      : m_intervals(intervals)
  {
  }

  double operator()(double x) const
  {
    for (const auto& [interval, polynomial] : m_intervals)
    {
      if (interval.contains(x))
      {
        return polynomial(x);
      }
    }

    // FIXME: throw an exception
    return 0;
  }

 private:
  std::vector<IntervalPolynomial> m_intervals;
};

}  // namespace Uni
