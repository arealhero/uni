#pragma once

#include <LibSLE/Solver.hh>
#include <iostream>

namespace Uni
{

class GaussSolver : public Solver
{
 public:
  [[nodiscard]] auto get_name() const -> std::string override
  {
    return std::string{"Gauss"};
  }
  // FIXME: remove NOLINT
  [[nodiscard]] constexpr auto solve(
      const Matrix& A,  // NOLINT(bugprone-easily-swappable-parameters,-warnings-as-errors)
      const Matrix& b,
      [[maybe_unused]] const double eps = DEFAULT_EPS) const -> Matrix override
  {
    auto A_copy = A;
    auto b_copy = b;

    down(A_copy, b_copy);
    up(A_copy, b_copy);

    return b_copy;
  }

 private:
  constexpr static void down(Matrix& A, Matrix& b)
  {
    for (std::size_t index = 0; index < A.size(); ++index)
    {
      // 1. Find max element in column
      std::size_t row_to_swap = index;
      auto max_element = A.at(index, index);
      for (std::size_t row = index + 1; row < A.size(); ++row)
      {
        auto element = A.at(row, index);
        if (std::abs(element) > std::abs(max_element))
        {
          max_element = element;
          row_to_swap = row;
        }
      }

      // 2. Swap current row with that row
      if (row_to_swap != index)
      {
        A.swap_rows(index, row_to_swap);
        b.swap_rows(index, row_to_swap);
      }

      // 3. Normalize row
      auto divider = A.at(index, index);

      for (std::size_t col = index; col < A.size(); ++col)
      {
        A.at(index, col) /= divider;
      }

      b.at(index) /= divider;

      // 4. Subtract that row from rows below
      for (std::size_t row = index + 1; row < A.size(); ++row)
      {
        auto factor = A.at(row, index);

        for (std::size_t col = index; col < A.size(); ++col)
        {
          A.at(row, col) -= factor * A.at(index, col);
        }

        b.at(row) -= factor * b.at(index);
      }
    }
  }

  // FIXME: remove NOLINT
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters,-warnings-as-errors)
  constexpr static void up(Matrix& A, Matrix& b)
  {
    for (std::size_t index = A.size() - 1; index != 0; --index)
    {
      for (std::size_t row = index; row != 0; --row)
      {
        auto factor = A.at(row - 1, index);

        for (std::size_t col = index; col < A.size(); ++col)
        {
          A.at(row - 1, col) -= factor * A.at(index, col);
        }

        b.at(row - 1) -= factor * b.at(index);
      }
    }
  }
};

}  // namespace Uni
