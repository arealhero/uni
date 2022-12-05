set(SOURCES
  LeastSquaresApproximator.cc
)

uni_lib(LibApproximation approximator)
target_link_libraries(LibApproximation PRIVATE LibMath)
