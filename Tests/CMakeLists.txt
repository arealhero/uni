include(GoogleTest)

enable_testing()

add_executable(Tests
  LibMath/Polynomial.specs.cc
  LibSLE/GaussSolver.specs.cc
)
target_include_directories(Tests PRIVATE ../include)
target_link_libraries(
  Tests
  GTest::gtest_main
  LibMath
)

gtest_discover_tests(Tests)
