#pragma once

#include <LibMath/Function.hh>
#include <LibMath/Interval.hh>
#include <LibMath/Polynomial.hh>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Uni
{

class Spline : public Function
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

  double operator()(double x) const override
  {
    for (const auto& [interval, polynomial] : m_polynomials)
    {
      if (interval.contains(x))
      {
        return polynomial(x);
      }
    }

    throw std::runtime_error{"x does not match any interval"};
  }

  const std::vector<IntervalPolynomial>& get_polynomials() const
  {
    return m_polynomials;
  }

 private:
  std::vector<IntervalPolynomial> m_polynomials;
};

std::ostream& operator<<(std::ostream& out, const Spline& spline);

}  // namespace Uni
