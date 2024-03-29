#pragma once

#include <VS/Concepts.hh>

namespace Uni
{

template <VS::Number T>
constexpr auto low_pass_filter(const T value, const T cutoff) -> T
{
  return (value < cutoff) ? value : cutoff;
}

template <VS::Number T>
constexpr auto high_pass_filter(const T value, const T cutoff) -> T
{
  return (value > cutoff) ? value : cutoff;
}

template <VS::Number T>
constexpr auto band_pass_filter(const T value,
                                const T low_cutoff,
                                const T high_cutoff) -> T
{
  return low_pass_filter(high_pass_filter(value, low_cutoff), high_cutoff);
}

}  // namespace Uni
