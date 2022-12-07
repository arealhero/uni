include(GoogleTest)

enable_testing()

add_executable(Tests
  LibApproximation/LeastSquaresApproximator.specs.cc
  LibInterpolation/Spline/Cubic.specs.cc
  LibInterpolation/Spline/Linear.specs.cc
  LibInterpolation/Spline/Quadratic.specs.cc
  LibMath/Polynomial.specs.cc
  LibSLE/GaussSolver.specs.cc
  LibStatistics/Average.specs.cc
)

target_link_libraries(Tests
  GTest::gtest_main
  LibApproximation
  LibInterpolation
  LibMath
)

gtest_discover_tests(Tests)
