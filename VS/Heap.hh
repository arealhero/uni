#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <vector>

namespace VS
{

template <typename T>
concept HeapTraits = requires(typename T::Type lhs, typename T::Type rhs)
{
  typename T::Type;
  {
    T::compare(lhs, rhs)
    } -> std::same_as<bool>;
};

template <HeapTraits Traits>
class Heap
{
 public:
  using ContainedType = typename Traits::Type;
  using SizeType = typename std::vector<ContainedType>::size_type;

  constexpr Heap() = default;
  constexpr Heap(std::initializer_list<ContainedType>&& values)
      : m_values(std::move(values))
  {
    std::make_heap(m_values.begin(), m_values.end(), compare);
  }

  constexpr void clear() { m_values.clear(); }
  [[nodiscard]] constexpr auto size() const -> SizeType
  {
    return m_values.size();
  }
  [[nodiscard]] constexpr auto empty() const -> bool { return size() == 0; }

  constexpr void push(const ContainedType& value)
  {
    m_values.push_back(value);
    std::push_heap(m_values.begin(), m_values.end(), compare);
  }

  [[nodiscard]] constexpr auto peek() const -> const ContainedType&
  {
    assert(!m_values.empty());
    return m_values.front();
  }

  constexpr void push_range(auto&& range)
  {
    for (const auto& element : range)
    {
      push(element);
    }
  }

  constexpr auto pop() -> ContainedType
  {
    std::pop_heap(m_values.begin(), m_values.end(), compare);
    const auto popped_element = m_values.back();
    m_values.pop_back();
    return popped_element;
  }
  [[nodiscard]] constexpr auto begin() { return m_values.begin(); }
  [[nodiscard]] constexpr auto end() { return m_values.end(); }

  [[nodiscard]] constexpr auto begin() const { return m_values.begin(); }
  [[nodiscard]] constexpr auto end() const { return m_values.end(); }

 private:
  std::vector<ContainedType> m_values;

  static auto compare(const ContainedType& lhs, const ContainedType& rhs)
      -> bool
  {
    return Traits::compare(lhs, rhs);
  }
};

template <typename ContainedType>
struct MaxHeapType
{
  using Type = ContainedType;

  constexpr static auto compare(const Type& lhs, const Type& rhs) -> bool
  {
    return lhs < rhs;
  }
};

template <typename ContainedType>
struct MinHeapType
{
  using Type = ContainedType;

  constexpr static auto compare(const Type& lhs, const Type& rhs) -> bool
  {
    return lhs > rhs;
  }
};

template <typename ContainedType>
using MaxHeap = Heap<MaxHeapType<ContainedType>>;

template <typename ContainedType>
using MinHeap = Heap<MinHeapType<ContainedType>>;

}  // namespace VS
