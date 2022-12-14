#pragma once

#include <memory>

namespace Uni
{

template <typename ValueType>
class SequenceGenerator;

template <typename ValueType>
class SequenceIterator
{
 public:
  SequenceIterator(SequenceGenerator<ValueType>* generator, std::size_t n)
      : m_generator(generator), n(n)
  {
  }

  [[nodiscard]] auto operator*() const -> ValueType
  {
    return m_generator->get(n);
  }

  auto operator++() -> SequenceIterator<ValueType>&
  {
    ++n;
    return *this;
  }

 private:
  SequenceGenerator<ValueType>* m_generator;
  std::size_t n;

  template <typename V>
  friend auto operator!=(const SequenceIterator<V>& lhs,
                         const SequenceIterator<V>& rhs) -> bool;
};

template <typename ValueType>
auto operator!=(const SequenceIterator<ValueType>& lhs,
                const SequenceIterator<ValueType>& rhs) -> bool
{
  return lhs.n != rhs.n;
}

template <typename ValueType>
class SequenceGenerator
{
 public:
  using iterator = SequenceIterator<ValueType>;

  // FIXME: remove NOLINT
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters,-warnings-as-errors)
  SequenceGenerator(ValueType a, ValueType b, std::size_t n) : a(a), b(b), n(n)
  {
  }
  virtual ~SequenceGenerator() = default;

  [[nodiscard]] virtual auto get(std::size_t index) const -> ValueType = 0;

  [[nodiscard]] auto begin() -> iterator { return iterator(this, 0); }
  [[nodiscard]] auto end() -> iterator { return iterator(this, n); }

 protected:
  // FIXME: remove NOLINT
  ValueType a, b;  // NOLINT(misc-non-private-member-variables-in-classes)
  std::size_t n;   // NOLINT(misc-non-private-member-variables-in-classes)
};

}  // namespace Uni
