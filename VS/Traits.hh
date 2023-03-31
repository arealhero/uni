#pragma once

namespace VS
{

template <typename T>
struct __RemoveConst
{
  using Type = T;
};
template <typename T>
struct __RemoveConst<const T>
{
  using Type = T;
};
template <typename T>
using RemoveConst = typename __RemoveConst<T>::Type;

template <typename T>
struct __RemoveVolatile
{
  using Type = T;
};
template <typename T>
struct __RemoveVolatile<volatile T>
{
  using Type = T;
};
template <typename T>
using RemoveVolatile = typename __RemoveVolatile<T>::Type;

template <typename T>
using RemoveCV = RemoveConst<RemoveVolatile<T>>;

template <typename T>
inline constexpr bool __IsFloatingPoint = false;

template <>
inline constexpr bool __IsFloatingPoint<float> = true;
template <>
inline constexpr bool __IsFloatingPoint<double> = true;
template <>
inline constexpr bool __IsFloatingPoint<long double> = true;

template <typename T>
inline constexpr bool IsFloatingPoint = __IsFloatingPoint<RemoveCV<T>>;

template <typename T>
inline constexpr bool __IsUnsigned = false;

template <>
inline constexpr bool __IsUnsigned<bool> = true;
template <>
inline constexpr bool __IsUnsigned<unsigned char> = true;
template <>
inline constexpr bool __IsUnsigned<unsigned short> = true;
template <>
inline constexpr bool __IsUnsigned<unsigned int> = true;
template <>
inline constexpr bool __IsUnsigned<unsigned long> = true;
template <>
inline constexpr bool __IsUnsigned<unsigned long long> = true;

template <typename T>
inline constexpr bool IsUnsigned = __IsUnsigned<RemoveCV<T>>;

template <typename T>
struct __MakeUnsigned
{
  using Type = void;
};

template <>
struct __MakeUnsigned<bool>
{
  using Type = bool;
};

template <>
struct __MakeUnsigned<char>
{
  using Type = unsigned char;
};
template <>
struct __MakeUnsigned<signed char>
{
  using Type = unsigned char;
};
template <>
struct __MakeUnsigned<signed short>
{
  using Type = unsigned short;
};
template <>
struct __MakeUnsigned<signed int>
{
  using Type = unsigned int;
};
template <>
struct __MakeUnsigned<signed long>
{
  using Type = unsigned long;
};
template <>
struct __MakeUnsigned<signed long long>
{
  using Type = unsigned long long;
};

template <>
struct __MakeUnsigned<unsigned char>
{
  using Type = unsigned char;
};
template <>
struct __MakeUnsigned<unsigned short>
{
  using Type = unsigned short;
};
template <>
struct __MakeUnsigned<unsigned int>
{
  using Type = unsigned int;
};
template <>
struct __MakeUnsigned<unsigned long>
{
  using Type = unsigned long;
};
template <>
struct __MakeUnsigned<unsigned long long>
{
  using Type = unsigned long long;
};

template <typename T>
using MakeUnsigned = typename __MakeUnsigned<T>::Type;

template <typename T>
inline constexpr bool __IsIntegral = false;

template <>
inline constexpr bool __IsIntegral<bool> = true;
template <>
inline constexpr bool __IsIntegral<unsigned char> = true;
template <>
inline constexpr bool __IsIntegral<unsigned short> = true;
template <>
inline constexpr bool __IsIntegral<unsigned int> = true;
template <>
inline constexpr bool __IsIntegral<unsigned long> = true;
template <>
inline constexpr bool __IsIntegral<unsigned long long> = true;

template <typename T>
inline constexpr bool IsIntegral = __IsIntegral<MakeUnsigned<RemoveCV<T>>>;

template <typename T>
inline constexpr bool IsNumber = IsFloatingPoint<T> || IsIntegral<T>;

}  // namespace VS
