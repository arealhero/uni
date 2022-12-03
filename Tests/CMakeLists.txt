include(GoogleTest)

enable_testing()

add_executable(Tests
  LibInterpolation/Spline/Cubic.specs.cc
  LibInterpolation/Spline/Linear.specs.cc
  LibInterpolation/Spline/Quadratic.specs.cc
  LibMath/Polynomial.specs.cc
  LibSLE/GaussSolver.specs.cc
)

target_link_libraries(Tests
  GTest::gtest_main
  LibInterpolation
  LibMath
)

gtest_discover_tests(Tests)
