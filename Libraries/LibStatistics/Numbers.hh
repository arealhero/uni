#pragma once

#include <VS/Concepts.hh>
#include <VS/Vector.hh>
#include <cassert>

namespace Uni
{

template <VS::Arithmetic T>
constexpr auto mean(const VS::Vector<T>& numbers) -> T
{
  // FIXME: remove cast
  return numbers.reduce(T{0}, std::plus<>{}) / static_cast<T>(numbers.size());
}

template <VS::Arithmetic T>
constexpr auto min(const VS::Vector<T>& numbers) -> T
{
  assert(numbers.size() > 0);
  return numbers.reduce(numbers.at(0),
                        [](const T& lhs, const T& rhs) -> T
                        { return lhs < rhs ? lhs : rhs; });
}

template <VS::Arithmetic T>
constexpr auto max(const VS::Vector<T>& numbers) -> T
{
  assert(numbers.size() > 0);
  return numbers.reduce(numbers.at(0),
                        [](const T& lhs, const T& rhs) -> T
                        { return lhs > rhs ? lhs : rhs; });
}

}  // namespace Uni
