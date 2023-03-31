#pragma once

#include <LibMath/Constants.hh>
#include <VS/Concepts.hh>
#include <VS/Types.hh>
#include <cassert>  // FIXME: stop using std headers
#include <limits>
#include <numbers>

constexpr auto sign(const VS::FloatingPoint auto x)
  requires VS::IsUnsigned<decltype(x)>
{
  using Type = VS::RemoveCV<decltype(x)>;

  return (x > 0) ? Type{1.} : Type{-1.};
}
constexpr auto sign(const VS::FloatingPoint auto x)
{
  using Type = VS::RemoveCV<decltype(x)>;
  return Type{1.};
}

constexpr auto abs(const VS::FloatingPoint auto x)
{
  return (x > 0) ? x : -x;
}

constexpr auto abs(const VS::Integral auto x)
{
  assert(x != std::numeric_limits<decltype(x)>::min());
  return (x > 0) ? x : -x;
}

constexpr auto power(VS::Number auto base, VS::Integral auto pow)
{
  using Type = VS::RemoveCV<decltype(base)>;

  assert(!(base == 0 && pow == 0) && "0 in the 0th power is undefined");

  base = (pow < 0) ? (1 / base) : base;
  pow = abs(pow);

  Type result{1};

  while (pow > 0)
  {
    result *= base;
    --pow;
  }

  return result;
}

// FIXME: implement memoization
template <VS::Integral Type>
constexpr Type factorial(const Type x)
{
  assert(x >= 0);
  return (x == 0) ? 1 : x * factorial(x - 1);
}

/**
 * См. Демидович - Основы вычислительной математики, 1966, стр. 104
 **/
template <VS::Number Type, VS::FloatingPoint EpsType = f64>
constexpr Type sqrt(const Type x, const EpsType eps = 1e-12)
{
  assert(x >= 0);

  if (x == 0)
  {
    return 0;
  }

  Type prev;
  Type next = x;

  do
  {
    prev = next;
    next = (prev + x / prev) / 2;
  } while (abs(prev - next) > eps);

  return next;
}

/**
 * См. Демидович - Основы вычислительной математики, 1966, стр. 88
 **/
template <VS::Number Type, VS::FloatingPoint EpsType = f64>
constexpr Type exp(const Type x, const EpsType eps = 1e-12)
{
  const auto whole_part = static_cast<i64>(x);
  // FIXME: get rid of potential implicit cast
  const auto decimal_part = x - whole_part;

  auto result = Type{1};

  if (whole_part != 0)
  {
    using Constants::e;
    using Constants::inv_e;

    const auto base = (whole_part > 0) ? e : inv_e;

    result = power(base, abs(whole_part));
  }

  if (decimal_part != 0)
  {
    i64 n = 1;
    while ((power(decimal_part, n + 1) / (n * factorial(n))) >= eps)
    {
      ++n;
    }

    auto decimal_result = Type{0};
    for (i64 i = 0; i <= n; ++i)
    {
      const auto temp = power(decimal_part, i) / factorial(i);
      decimal_result += temp;
    }

    result *= decimal_result;
  }

  return result;
}

template <VS::Number Type, VS::FloatingPoint EpsType = f64>
constexpr Type cos(Type x, EpsType eps = 1e-12);

/**
 * См. Демидович - Основы вычислительной математики, 1966, стр. 95
 **/
template <VS::Number Type, VS::FloatingPoint EpsType = f64>
constexpr Type sin(Type x, const EpsType eps = 1e-12)
{
  using namespace Constants;

  // FIXME: ok, floats are symmetrical so DBL_MIN := -DBL_MAX
  //        (ref:
  //        https://stackoverflow.com/questions/1153548/minimum-double-value-in-c-c/1153574#1153574),
  //        but what about other situations (like what if x is infinity)?
  if (x < 0)
  {
    return -sin(-x, eps);
  }

  // Нормализация
  const auto r = static_cast<u64>(x / two_pi);
  x -= r * two_pi;

  // См. формулы приведения
  if (x > 3.0 * half_pi)
  {
    return -cos(x - 3.0 * half_pi, eps);
  }

  if (x > pi)
  {
    return -sin(x - pi, eps);
  }

  if (x > half_pi)
  {
    return cos(x - half_pi, eps);
  }

  if (x > pi_over_4)
  {
    return cos(half_pi - x, eps);
  }

  if (x == 0.)
  {
    return Type{0.};
  }

  // x in (0; pi/2]
  Type result = 0.0;
  Type next = x;
  i32 k = 1;

  do
  {
    result += next;
    next *= -x * x / ((2 * k) * (2 * k + 1));
    k += 1;
  } while (abs(next) > eps);

  return result;
}

/**
 * См. Демидович - Основы вычислительной математики, 1966, стр. 95
 **/
template <VS::Number Type, VS::FloatingPoint EpsType>
constexpr Type cos(Type x, const EpsType eps)
{
  using namespace Constants;

  if (x < 0)
  {
    return cos(-x, eps);
  }

  // Нормализация
  const auto r = static_cast<u64>(x / two_pi);
  x -= r * two_pi;

  // См. формулы приведения
  if (x > 3.0 * half_pi)
  {
    return sin(x - 3.0 * half_pi, eps);
  }

  if (x > pi)
  {
    return -cos(x - pi, eps);
  }

  if (x > half_pi)
  {
    return -sin(x - half_pi, eps);
  }

  if (x > pi_over_4)
  {
    return sin(half_pi - x, eps);
  }

  if (x == 0.)
  {
    return Type{1.};
  }

  // x in (0; pi/2]
  Type result = 0.0;
  Type next = 1.;
  i32 k = 1;

  do
  {
    result += next;
    next *= -x * x / ((2 * k) * (2 * k - 1));
    k += 1;
  } while (abs(next) > eps);

  return result;
}
