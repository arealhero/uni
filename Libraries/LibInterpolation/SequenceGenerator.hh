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

  ValueType operator*() const { return m_generator->get(n); }

  SequenceIterator<ValueType>& operator++()
  {
    ++n;
    return *this;
  }

 private:
  SequenceGenerator<ValueType>* m_generator;
  std::size_t n;

  template <typename V>
  friend bool operator!=(const SequenceIterator<V>& lhs,
                         const SequenceIterator<V>& rhs);
};

template <typename ValueType>
bool operator!=(const SequenceIterator<ValueType>& lhs,
                const SequenceIterator<ValueType>& rhs)
{
  return lhs.n != rhs.n;
}

template <typename ValueType>
class SequenceGenerator
{
 public:
  using iterator = SequenceIterator<ValueType>;

  SequenceGenerator(ValueType a, ValueType b, std::size_t n) : a(a), b(b), n(n)
  {
  }
  virtual ~SequenceGenerator() = default;

  virtual ValueType get(std::size_t index) const = 0;

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, n); }

 protected:
  ValueType a, b;
  std::size_t n;
};

}  // namespace Uni
