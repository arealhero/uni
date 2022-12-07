#pragma once

#include <type_traits>

namespace VS
{

template <typename From, typename To>
concept IsConvertibleTo = std::is_convertible_v<From, To>;

template <typename T, typename... Ts>
using AllSame = std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>;

}  // namespace VS
