include(GoogleTest)

enable_testing()

add_executable(Tests
  # LibApproximation/LeastSquaresApproximator.specs.cc
  # LibInterpolation/Spline/Cubic.specs.cc
  # LibInterpolation/Spline/Linear.specs.cc
  # LibInterpolation/Spline/Quadratic.specs.cc
  LibMath/Functions/Cosine.specs.cc
  LibMath/Functions/Exponent.specs.cc
  LibMath/Functions/Sine.specs.cc
  LibMath/Functions/SquareRoot.specs.cc
  LibMath/Functions/Trigonometry.specs.cc
  LibMath/Algebra.specs.cc
  # LibMath/Polynomial.specs.cc
  # LibOptimization/Schedule.specs.cc
  # LibSLE/GaussSolver.specs.cc
  # LibStatistics/Average.specs.cc
)

target_link_libraries(Tests
  GTest::gtest_main
  # LibInterpolation
  LibMath
)

gtest_discover_tests(Tests)
