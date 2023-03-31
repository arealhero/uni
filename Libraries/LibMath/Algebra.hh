#pragma once

#include <LibMath/Math.hh>
#include <VS/Types.hh>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <tuple>

namespace LibMath::Numerical::Algebra
{

// FIXME: change arguments to Uni::Polynomial
constexpr std::tuple<f64, f64, f64> find_roots(f64 a,
                                               f64 b,
                                               f64 c,
                                               f64 d) noexcept(false)
{
  using Constants::pi;

  const f64 q = 0.5 * ((2 * b * b * b) / (27 * a * a * a) -
                       (b * c) / (3 * a * a) + d / a);
  const f64 p = (3 * a * c - b * b) * (9 * a * a);

  const f64 D = q * q + p * p * p;

  if (D > 0)
  {
    throw std::runtime_error{"D > 0"};
  }

  const f64 r = sign(q) * std::sqrt(std::abs(p));

  if (p >= 0.)
  {
    throw std::runtime_error{"p >= 0"};
  }

  const f64 phi = std::acos(q / (r * r * r));

  return {
      -2 * r * std::cos(phi / 3) - b / (3 * a),
      +2 * r * std::cos(pi / 3 - phi / 3) - b / (3 * a),
      +2 * r * std::cos(pi / 3 + phi / 3) - b / (3 * a),
  };
}

}  // namespace LibMath::Numerical::Algebra
