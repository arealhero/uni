#pragma once

#include <VS/Traits.hh>
#include <VS/Types.hh>
#include <algorithm>
#include <cassert>
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
  constexpr Vector(const std::size_t size,
                   const ContainedType& element = ContainedType{})
      : m_data(size, element)
  {
  }
  constexpr Vector(const std::size_t size,
                   ContainedType&& element = ContainedType{})
      : m_data(size, std::move(element))
  {
  }
  constexpr Vector(std::initializer_list<ContainedType>&& elements)
      : m_data(std::move(elements))
  {
  }

  // FIXME: test if every Args is convertible to vector
  template <typename... Args>
  constexpr static auto merge(Args&&... elements) -> Vector<ContainedType>
  {
    auto result = Vector<ContainedType>{};

    // FIXME: move this dirty hack somewhere else
    [](auto...) {
    }((result.push_back(elements), 0)...);

    return result;
  }

  auto get_std() -> auto& { return m_data; }

  constexpr void push_back(const Vector<ContainedType>& other)
  {
    for (const auto& element : other.m_data)
    {
      m_data.push_back(element);
    }
  }

  template <typename... Args>
  constexpr void insert(
      const typename std::vector<ContainedType>::iterator& it,
      Args&&... args)
  {
    m_data.insert(it, std::forward<Args>(args)...);
  }

  constexpr void push_back(const ContainedType& element)
  {
    m_data.push_back(element);
  }

  constexpr void push_back(ContainedType&& element)
  {
    m_data.push_back(std::move(element));
  }

  template <typename... Args>
  constexpr void emplace_back(Args&&... args)
  {
    m_data.emplace_back(std::forward<Args>(args)...);
  }

  constexpr void sort(auto&& comparator = std::greater<>{})
  {
    std::sort(m_data.begin(), m_data.end(), comparator);
  }

  [[nodiscard]] constexpr auto at(const Size index) -> ContainedType&
  {
    return m_data.at(index);
  }
  [[nodiscard]] constexpr auto at(const Size index) const
      -> const ContainedType&
  {
    return m_data.at(index);
  }

  [[nodiscard]] constexpr auto back() const -> const ContainedType&
  {
    return m_data.back();
  }
  [[nodiscard]] constexpr auto back() -> ContainedType&
  {
    return m_data.back();
  }

  [[nodiscard]] constexpr auto find_max_element() const
      -> std::pair<Size, ContainedType>
  {
    assert(size() > 0);

    Size max_index = 0;
    auto max_value = at(max_index);
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

  [[nodiscard]] constexpr auto size() const -> Size { return m_data.size(); }
  [[nodiscard]] constexpr auto is_empty() const -> bool
  {
    return m_data.empty();
  }

  constexpr void reserve(const Size size) { m_data.reserve(size); }

  // FIXME: remove NOLINT
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  [[nodiscard]] constexpr auto subvector(const Index start,
                                         const Size count) const
  {
    auto result = Vector<ContainedType>{};
    result.reserve(count);
    for (Index i = start; i < start + count; ++i)
    {
      result.push_back(at(i));
    }
    return result;
  }

  [[nodiscard]] constexpr auto filter(auto&& function) const
      -> Vector<ContainedType>
  {
    auto result = Vector<ContainedType>{};
    for (const auto& element : m_data)
    {
      if (function(element))
      {
        result.push_back(element);
      }
    }
    return result;
  }

  template <typename ReturnedType>
  [[nodiscard]] constexpr auto map(auto&& function) const
      -> Vector<ReturnedType>
  {
    auto result = Vector<ReturnedType>{};
    result.reserve(size());
    for (const auto& element : m_data)
    {
      result.push_back(function(element));
    }
    return result;
  }

  [[nodiscard]] constexpr auto reduce(
      const ContainedType& init,
      std::function<ContainedType(const ContainedType&,
                                  const ContainedType&)>&& function) const
      -> ContainedType
  {
    return std::reduce(m_data.begin(), m_data.end(), init, function);
  }
  [[nodiscard]] constexpr auto reduce(
      ContainedType&& init,
      std::function<ContainedType(const ContainedType&,
                                  const ContainedType&)>&& function) const
      -> ContainedType
  {
    return std::reduce(
        m_data.begin(), m_data.end(), std::move(init), function);
  }

  [[nodiscard]] constexpr auto reduce_to_index(
      std::function<bool(const ContainedType&, const ContainedType&)>&&
          function) const -> VS::Index
  {
    assert(!is_empty());

    VS::Index best_index = 0;
    for (VS::Index i = 1; i < size(); ++i)
    {
      if (function(at(best_index), at(i)))
      {
        best_index = i;
      }
    }

    return best_index;
  }

  [[nodiscard]] constexpr auto begin() { return m_data.begin(); }
  [[nodiscard]] constexpr auto end() { return m_data.end(); }

  [[nodiscard]] constexpr auto begin() const { return m_data.begin(); }
  [[nodiscard]] constexpr auto end() const { return m_data.end(); }

 private:
  // FIXME: change to VS::Vector
  std::vector<ContainedType> m_data;
};

using Indices = Vector<Index>;

}  // namespace VS
