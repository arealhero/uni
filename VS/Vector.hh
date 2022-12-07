#pragma once

#include <VS/Traits.hh>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <numeric>
#include <utility>
#include <vector>

namespace VS
{

template <typename ContainedType>
class Vector
{
 public:
  using Size = std::size_t;

  constexpr Vector() = default;
  constexpr Vector(std::initializer_list<ContainedType>&& elements)
      : m_data(std::move(elements))
  {
  }

  template <typename... Args>
  constexpr static Vector<ContainedType> merge(Args&&... elements)
  {
    auto result = Vector<ContainedType>{};

    // FIXME: move this dirty hack somewhere else
    (void)(int[]){(result.push_back(elements), 0)...};

    return result;
  }

  constexpr void push_back(const Vector<ContainedType>& other)
  {
    for (const auto& element : other.m_data)
      m_data.push_back(element);
  }

  constexpr void push_back(const ContainedType& element)
  {
    m_data.push_back(element);
  }

  constexpr void push_back(ContainedType&& element)
  {
    m_data.push_back(std::move(element));
  }

  constexpr ContainedType& at(const Size index) { return m_data.at(index); }
  constexpr const ContainedType& at(const Size index) const
  {
    return m_data.at(index);
  }

  constexpr std::pair<Size, ContainedType> find_max_element() const
  {
    assert(size() > 0);

    Size max_index = 0;
    ContainedType max_value = at(max_index);
    for (Size index = 1; index < size(); ++index)
    {
      const auto value = at(index);
      if (value > max_value)
      {
        max_index = index;
        max_value = value;
      }
    }
    return {max_index, max_value};
  }

  constexpr Size size() const { return m_data.size(); }
  constexpr void reserve(const Size size) { m_data.reserve(size); }

  constexpr Vector<ContainedType> filter(auto&& function) const
  {
    auto result = Vector<ContainedType>{};
    for (const auto& element : m_data)
      if (function(element))
        result.push_back(element);
    return result;
  }

  template <typename ReturnedType>
  constexpr Vector<ReturnedType> map(auto&& function) const
  {
    auto result = Vector<ReturnedType>{};
    result.reserve(size());
    for (const auto& element : m_data)
      result.push_back(function(element));
    return result;
  }

  constexpr ContainedType reduce(ContainedType&& init, auto&& function) const
  {
    return std::reduce(m_data.begin(), m_data.end(), init, function);
  }

  constexpr auto begin() { return m_data.begin(); }
  constexpr auto end() { return m_data.end(); }

  constexpr auto begin() const { return m_data.begin(); }
  constexpr auto end() const { return m_data.end(); }

 private:
  std::vector<ContainedType> m_data;
};

}  // namespace VS
