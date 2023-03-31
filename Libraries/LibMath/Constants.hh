#pragma once

#include <VS/Concepts.hh>
#include <VS/Types.hh>

namespace Constants
{

template <VS::FloatingPoint T>
inline constexpr T e_v = 2.718281828459045235360287471352662498L;

template <VS::FloatingPoint T>
inline constexpr T inv_e_v = 0.367879441171442321595523770161460867L;

template <VS::FloatingPoint T>
inline constexpr T two_pi_v = 6.283185307179586476925286766559005768L;

template <VS::FloatingPoint T>
inline constexpr T pi_v = 3.141592653589793238462643383279502884L;

template <VS::FloatingPoint T>
inline constexpr T half_pi_v = 1.570796326794896619231321691639751442L;

template <VS::FloatingPoint T>
inline constexpr T pi_over_4_v = 0.785398163397448309615660845819875721L;

template <VS::FloatingPoint T>
inline constexpr T inv_pi_v = 0.318309886183790671537767526745028724L;

template <VS::FloatingPoint T>
inline constexpr T sqrt_2_v = 1.4142135623730950488016887242096980786L;

template <VS::FloatingPoint T>
inline constexpr T sqrt_3_v = 1.7320508075688772935274463415058723669L;

inline constexpr f64 e = e_v<f64>;
inline constexpr f64 inv_e = inv_e_v<f64>;
inline constexpr f64 two_pi = two_pi_v<f64>;
inline constexpr f64 pi = pi_v<f64>;
inline constexpr f64 half_pi = half_pi_v<f64>;
inline constexpr f64 pi_over_4 = pi_over_4_v<f64>;
inline constexpr f64 inv_pi = inv_pi_v<f64>;
inline constexpr f64 sqrt_2 = sqrt_2_v<f64>;
inline constexpr f64 sqrt_3 = sqrt_3_v<f64>;

}  // namespace Constants
