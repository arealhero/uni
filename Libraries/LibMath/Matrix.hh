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
  using Value = double;
  using Size = std::size_t;

  constexpr explicit Matrix(const Size rows,
                            const Size cols,
                            const Value value = 0)
      : m_data(rows * cols, value), m_rows(rows), m_cols(cols)
  {
  }
  // FIXME: remove NOLINT
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters,-warnings-as-errors)
  constexpr Matrix(const Size rows,
                   const Size cols,
                   const std::initializer_list<Value>& numbers)
      : m_data(numbers), m_rows(rows), m_cols(cols)
  {
    assert(m_data.size() == m_rows * m_cols);
  }

  [[nodiscard]] constexpr auto rows() const -> Size { return m_rows; }
  [[nodiscard]] constexpr auto cols() const -> Size { return m_cols; }
  [[nodiscard]] constexpr auto is_square() const -> bool
  {
    return rows() == cols();
  }

  [[nodiscard]] constexpr auto size() const -> Size
  {
    assert(is_square());
    return m_rows;
  }

  [[nodiscard]] constexpr auto at(std::size_t i, std::size_t j) -> Value&
  {
    assert(i < rows() && j < cols());
    return m_data[i * cols() + j];
  }
  [[nodiscard]] constexpr auto at(std::size_t i, std::size_t j) const
      -> const Value&
  {
    assert(i < rows() && j < cols());
    return m_data[i * cols() + j];
  }

  [[nodiscard]] constexpr auto at(std::size_t index) -> Value&
  {
    assert((rows() == 1 || cols() == 1) && index < rows() * cols());
    return m_data[index];
  }
  [[nodiscard]] constexpr auto at(std::size_t index) const -> const Value&
  {
    assert((rows() == 1 || cols() == 1) && index < rows() * cols());
    return m_data[index];
  }

  constexpr auto operator+=(const Value number) -> Matrix&
  {
    apply_all([number](auto& element) { element += number; });
    return *this;
  }
  constexpr auto operator-=(const Value number) -> Matrix&
  {
    apply_all([number](auto& element) { element -= number; });
    return *this;
  }
  constexpr auto operator*=(const Value number) -> Matrix&
  {
    apply_all([number](auto& element) { element *= number; });
    return *this;
  }
  constexpr auto operator/=(const Value number) -> Matrix&
  {
    apply_all([number](auto& element) { element /= number; });
    return *this;
  }

  constexpr auto operator+=(const Matrix& other) -> Matrix&
  {
    assert(rows() == other.rows() && cols() == other.cols());

    for (std::size_t row = 0; row < rows(); ++row)
    {
      for (std::size_t col = 0; col < cols(); ++col)
      {
        at(row, col) += other.at(row, col);
      }
    }

    return *this;
  }
  constexpr auto operator-=(const Matrix& other) -> Matrix&
  {
    return *this += -other;
  }

  [[nodiscard]] constexpr auto norm_1() const -> Value
  {
    Value result{0};

    for (std::size_t j = 0; j < cols(); ++j)
    {
      Value sum{0};

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

  [[nodiscard]] constexpr auto norm_2() const -> Value
  {
    Value result{0};

    for (std::size_t j = 0; j < cols(); ++j)
    {
      Value sum{0};

      for (std::size_t i = 0; i < rows(); ++i)
      {
        auto element = at(i, j);
        sum += element * element;
      }

      if (sum > result)
      {
        result = sum;
      }
    }

    return std::sqrt(result);
  }

  [[nodiscard]] constexpr auto operator-() const -> Matrix
  {
    auto result = *this;
    result.apply_all([](auto& element) { element = -element; });
    return result;
  }

  constexpr operator Value() const
  {
    assert(rows() == 1 && cols() == 1);
    return at(0, 0);
  }

  [[nodiscard]] constexpr auto T() const -> Matrix
  {
    auto result = Matrix(cols(), rows());
    for (std::size_t row = 0; row < rows(); ++row)
    {
      for (std::size_t col = 0; col < cols(); ++col)
      {
        result.at(col, row) = at(row, col);
      }
    }
    return result;
  }

  constexpr void swap_rows(std::size_t i, std::size_t j)
  {
    assert(i < rows() && j < rows());

    if (i == j)
    {
      return;
    }

    for (std::size_t col = 0; col < cols(); ++col)
    {
      auto temp = at(i, col);
      at(i, col) = at(j, col);
      at(j, col) = temp;
    }
  }

  [[nodiscard]] constexpr auto get_data() const -> std::vector<Value>
  {
    return m_data;
  }

  [[nodiscard]] constexpr auto get_diagonal_dominance_factor() const
      -> std::optional<Value>
  {
    Value min_factor = std::abs(at(0, 0));

    for (std::size_t row = 0; row < rows(); ++row)
    {
      Value row_factor = 0;
      for (std::size_t col = 0; col < cols(); ++col)
      {
        auto current_value = std::abs(at(row, col));
        if (row == col)
        {
          row_factor += current_value;
        }
        else
        {
          row_factor -= current_value;
        }
      }

      if (row_factor < 0)
      {
        return std::nullopt;
      }

      if (row_factor < min_factor)
      {
        min_factor = row_factor;
      }
    }

    return min_factor;
  }

  [[nodiscard]] constexpr auto is_diagonally_dominant() const -> bool
  {
    return get_diagonal_dominance_factor().has_value();
  }

  [[nodiscard]] constexpr auto inv() const -> Matrix;

  constexpr static auto zero(const Size size) -> Matrix
  {
    return zero(size, size);
  }
  constexpr static auto zero(const Size rows, const Size cols) -> Matrix
  {
    return Matrix(rows, cols);
  }

  constexpr static auto identity(const Size size) -> Matrix
  {
    auto result = zero(size);
    for (std::size_t i = 0; i < size; ++i)
    {
      result.at(i, i) = 1;
    }
    return result;
  }

  constexpr static auto e(const Size size, const Size i) -> Matrix
  {
    assert(i < size);
    auto result = zero(size);
    result.at(i) = 1;
    return result;
  }
  constexpr static auto e_T(const Size size, const Size i) -> Matrix
  {
    return e(size, i).T();
  }

 private:
  std::vector<Value> m_data;
  Size m_rows;
  Size m_cols;

  constexpr void apply_all(auto&& func)
  {
    std::for_each(m_data.begin(), m_data.end(), func);
  }
};

constexpr auto operator+(const Matrix& lhs, const Matrix& rhs) -> Matrix
{
  assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());
  auto result = lhs;
  return result += rhs;
}
constexpr auto operator-(const Matrix& lhs, const Matrix& rhs) -> Matrix
{
  return lhs + -rhs;
}

constexpr auto operator==(const Matrix& lhs, const Matrix& rhs) -> bool
{
  if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
  {
    return false;
  }

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

constexpr auto operator*(const Matrix& lhs, const Matrix& rhs) -> Matrix
{
  assert(lhs.cols() == rhs.rows());
  const auto K = lhs.cols();

  auto result = Matrix::zero(lhs.rows(), rhs.cols());
  for (std::size_t row = 0; row < lhs.rows(); ++row)
  {
    for (std::size_t col = 0; col < rhs.cols(); ++col)
    {
      for (std::size_t k = 0; k < K; ++k)
      {
        result.at(row, col) += lhs.at(row, k) * rhs.at(k, col);
      }
    }
  }

  return result;
}

constexpr auto operator*(const typename Matrix::Value number,
                         const Matrix& mat) -> Matrix
{
  auto result = mat;
  return result *= number;
}

auto operator<<(std::ostream& out, const Matrix& matrix) -> std::ostream&;

}  // namespace Uni
