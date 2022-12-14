#pragma once

#include <VS/Traits.hh>

namespace VS
{

template <typename T>
concept Integral = IsIntegral<T>;

template <typename T>
concept FloatingPoint = IsFloatingPoint<T>;

template <typename T>
concept Arithmetic = IsArithmetic<T>;

}  // namespace VS
