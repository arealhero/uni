set(SOURCES
  Polynomial/Lagrange.cc
  Polynomial/Newton.cc
  Spline/Cubic.cc
  Spline/Linear.cc
  Spline/Quadratic.cc
)

uni_lib(LibInterpolation interpolation)
target_link_libraries(LibInterpolation PRIVATE LibMath)
