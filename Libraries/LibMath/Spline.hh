#pragma once

#include <utility>
#include <vector>

#include "Interval.hh"
#include "Polynomial.hh"

namespace Uni
{

class Spline
{
 public:
  using IntervalPolynomial = std::pair<Interval, Polynomial>;

  Spline(const std::vector<IntervalPolynomial>& intervals)
      : m_polynomials(intervals)
  {
  }
  Spline(const std::initializer_list<IntervalPolynomial>& intervals)
      : m_polynomials(intervals)
  {
  }

  double operator()(double x) const
  {
    for (const auto& [interval, polynomial] : m_polynomials)
    {
      if (interval.contains(x))
      {
        return polynomial(x);
      }
    }

    // FIXME: throw an exception
    return 0;
  }

  const std::vector<IntervalPolynomial>& get_polynomials() const
  {
    return m_polynomials;
  }

 private:
  std::vector<IntervalPolynomial> m_polynomials;
};

}  // namespace Uni
