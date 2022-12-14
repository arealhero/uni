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
  Spline(std::vector<IntervalPolynomial>&& intervals)
    : m_polynomials(std::move(intervals))
  {
  }
  Spline(const std::initializer_list<IntervalPolynomial>& intervals)
      : m_polynomials(intervals)
  {
  }

  [[nodiscard]] auto operator()(double x) const ->double override
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

  [[nodiscard]] auto get_polynomials() const -> const std::vector<IntervalPolynomial>&
  {
    return m_polynomials;
  }

 private:
  // FIXME: change std::vector to VS::Vector
  std::vector<IntervalPolynomial> m_polynomials;
};

auto operator<<(std::ostream& out, const Spline& spline) -> std::ostream&;

}  // namespace Uni
