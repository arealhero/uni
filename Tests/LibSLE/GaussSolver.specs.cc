#include <gtest/gtest.h>

#include <LibMath/Matrix.hh>
#include <LibSLE/GaussSolver.hh>

#include <memory>

TEST(GaussSolver, SimpleExample)
{
  const auto A = Uni::Matrix::identity(3);
  const auto b = Uni::Matrix(3, 1, 1.0);

  auto solver = std::make_shared<Uni::GaussSolver>();
  auto x = solver->solve(A, b, 1e-6);

  EXPECT_EQ(x, b);
}
