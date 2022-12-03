#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <vector>

namespace Uni
{

class Matrix
{
 public:
  using value_type = double;
  using size_type = std::size_t;

  constexpr explicit Matrix(const size_type rows,
                            const size_type cols,
                            const value_type value = 0)
      : m_data(rows * cols, value), m_rows(rows), m_cols(cols)
  {
  }
  constexpr Matrix(const size_type rows,
                   const size_type cols,
                   const std::initializer_list<value_type>& numbers)
      : m_data(numbers), m_rows(rows), m_cols(cols)
  {
    assert(m_data.size() == m_rows * m_cols);
  }

  constexpr size_type rows() const { return m_rows; }
  constexpr size_type cols() const { return m_cols; }
  constexpr bool is_square() const { return rows() == cols(); }

  constexpr size_type size() const
  {
    assert(is_square());
    return m_rows;
  }

  constexpr value_type& at(std::size_t i, std::size_t j)
  {
    assert(i < rows() && j < cols());
    return m_data[i * cols() + j];
  }
  constexpr const value_type& at(std::size_t i, std::size_t j) const
  {
    assert(i < rows() && j < cols());
    return m_data[i * cols() + j];
  }

  constexpr value_type& at(std::size_t index)
  {
    assert((rows() == 1 || cols() == 1) && index < rows() * cols());
    return m_data[index];
  }
  constexpr const value_type& at(std::size_t index) const
  {
    assert((rows() == 1 || cols() == 1) && index < rows() * cols());
    return m_data[index];
  }

  constexpr auto& operator+=(const value_type number)
  {
    apply_all([number](auto& element) { element += number; });
    return *this;
  }
  constexpr auto& operator-=(const value_type number)
  {
    apply_all([number](auto& element) { element -= number; });
    return *this;
  }
  constexpr auto& operator*=(const value_type number)
  {
    apply_all([number](auto& element) { element *= number; });
    return *this;
  }
  constexpr auto& operator/=(const value_type number)
  {
    apply_all([number](auto& element) { element /= number; });
    return *this;
  }

  constexpr auto& operator+=(const Matrix& other)
  {
    assert(rows() == other.rows() && cols() == other.cols());

    for (std::size_t row = 0; row < rows(); ++row)
      for (std::size_t col = 0; col < cols(); ++col)
        at(row, col) += other.at(row, col);

    return *this;
  }
  constexpr auto& operator-=(const Matrix& other) { return *this += -other; }

  constexpr value_type norm_1() const
  {
    value_type result{0};

    for (std::size_t j = 0; j < cols(); ++j)
    {
      value_type sum{0};

      for (std::size_t i = 0; i < rows(); ++i)
      {
        sum += std::abs(at(i, j));
      }

      if (sum > result)
      {
        result = sum;
      }
    }

    return result;
  }

  constexpr value_type norm_2() const
  {
    value_type result{0};

    for (std::size_t j = 0; j < cols(); ++j)
    {
      value_type sum{0};

      for (std::size_t i = 0; i < rows(); ++i)
      {
        auto element = at(i, j);
        sum += element * element;
      }

      if (sum > result)
        result = sum;
    }

    return std::sqrt(result);
  }

  constexpr Matrix operator-() const
  {
    auto result = *this;
    result.apply_all([](auto& element) { element = -element; });
    return result;
  }

  constexpr operator value_type() const
  {
    assert(rows() == 1 && cols() == 1);
    return at(0, 0);
  }

  constexpr Matrix T() const
  {
    auto result = Matrix(cols(), rows());
    for (std::size_t row = 0; row < rows(); ++row)
      for (std::size_t col = 0; col < cols(); ++col)
        result.at(col, row) = at(row, col);
    return result;
  }

  constexpr void swap_rows(std::size_t i, std::size_t j)
  {
    assert(i < rows() && j < rows());

    if (i == j)
      return;

    for (std::size_t col = 0; col < cols(); ++col)
    {
      auto temp = at(i, col);
      at(i, col) = at(j, col);
      at(j, col) = temp;
    }
  }

  constexpr std::optional<value_type> get_diagonal_dominance_factor() const
  {
    value_type min_factor = std::abs(at(0, 0));

    for (std::size_t row = 0; row < rows(); ++row)
    {
      value_type row_factor = 0;
      for (std::size_t col = 0; col < cols(); ++col)
      {
        auto current_value = std::abs(at(row, col));
        if (row == col)
          row_factor += current_value;
        else
          row_factor -= current_value;
      }

      if (row_factor < 0)
        return std::nullopt;

      if (row_factor < min_factor)
        min_factor = row_factor;
    }

    return min_factor;
  }

  constexpr bool is_diagonally_dominant() const
  {
    return get_diagonal_dominance_factor().has_value();
  }

  Matrix inv() const { throw new std::runtime_error{"Not implemented yet"}; }

  constexpr static Matrix zero(const size_type size)
  {
    return zero(size, size);
  }
  constexpr static Matrix zero(const size_type rows, const size_type cols)
  {
    return Matrix(rows, cols);
  }

  constexpr static Matrix identity(const size_type size)
  {
    auto result = zero(size);
    for (std::size_t i = 0; i < size; ++i)
      result.at(i, i) = 1;
    return result;
  }

  constexpr static Matrix e(const size_type size, const size_type i)
  {
    assert(i < size);
    auto result = zero(size);
    result.at(i) = 1;
    return result;
  }
  constexpr static Matrix e_T(const size_type size, const size_type i)
  {
    return e(size, i).T();
  }

 private:
  std::vector<value_type> m_data;
  size_type m_rows;
  size_type m_cols;

  constexpr void apply_all(auto&& func)
  {
    std::for_each(m_data.begin(), m_data.end(), func);
  }
};

constexpr Matrix operator+(const Matrix& lhs, const Matrix& rhs)
{
  assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());
  auto result = lhs;
  return result += rhs;
}
constexpr Matrix operator-(const Matrix& lhs, const Matrix& rhs)
{
  return lhs + -rhs;
}

constexpr bool operator==(const Matrix& lhs, const Matrix& rhs)
{
  if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
    return false;

  for (std::size_t row = 0; row < lhs.rows(); ++row)
  {
    for (std::size_t col = 0; col < lhs.cols(); ++col)
    {
      if (lhs.at(row, col) != rhs.at(row, col))
      {
        return false;
      }
    }
  }

  return true;
}

constexpr Matrix operator*(const Matrix& lhs, const Matrix& rhs)
{
  assert(lhs.cols() == rhs.rows());
  const auto K = lhs.cols();

  auto result = Matrix::zero(lhs.rows(), rhs.cols());
  for (std::size_t row = 0; row < lhs.rows(); ++row)
    for (std::size_t col = 0; col < rhs.cols(); ++col)
      for (std::size_t k = 0; k < K; ++k)
        result.at(row, col) += lhs.at(row, k) * rhs.at(k, col);

  return result;
}

constexpr Matrix operator*(const typename Matrix::value_type number,
                           const Matrix& mat)
{
  auto result = mat;
  return result *= number;
}

std::ostream& operator<<(std::ostream& out, const Matrix& matrix);

}  // namespace Uni
