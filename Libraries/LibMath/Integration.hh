#pragma once

#include <VS/Types.hh>

namespace LibMath::Numerical::Integration
{

using Function = f64 (*)(f64);
using Method = f64 (*)(Function, f64, f64);

constexpr f64 uniform_partition(Function f, f64 a, f64 b,
    u32 number_of_partitions, Method method)
{
  // FIXME: ok, floats are symmetrical so DBL_MIN := -DBL_MAX
  //        (ref: https://stackoverflow.com/questions/1153548/minimum-double-value-in-c-c/1153574#1153574),
  //        but what about other situations (like what if b is infinity)?
  const f64 step = (b - a) / number_of_partitions;

  f64 result = 0.0;
  for (u32 i = 0; i < number_of_partitions; ++i)
  {
    const f64 left = a + i * step;
    const f64 right = left + step;

    result += method(f, left, right);
  }

  return result;
}

constexpr f64 left_rectangle(Function f, f64 a, f64 b)
{
  // FIXME: add validation & error handling
  return (b - a) * f(a);
}

constexpr f64 right_rectangle(Function f, f64 a, f64 b)
{
  // FIXME: add validation & error handling
  return (b - a) * f(b);
}

constexpr f64 middle_rectangle(Function f, f64 a, f64 b)
{
  // FIXME: add validation & error handling
  const f64 middle = (a + b) / 2.0;
  return (b - a) * f(middle);
}

constexpr f64 trapezoid(Function f, f64 a, f64 b)
{
  // FIXME: add validation & error handling
  return (b - a) * (f(a) + f(b)) / 2.0;
}

constexpr f64 simpson(Function f, f64 a, f64 b)
{
  // FIXME: add validation & error handling
  const f64 middle = (a + b) / 2.0;
  return (b - a) * (f(a) + 4.0 * f(middle) + f(b)) / 6.0;
}

/* constexpr f64 newton_cotes(Function p, Function f, f64 a, f64 b) */
/* { */
/*   return 0.; */
/* } */

/* constexpr f64 gauss(Function p, Function f, f64 a, f64 b) */
/* { */
/*   return 0.; */
/* } */

}

