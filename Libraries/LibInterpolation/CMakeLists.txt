set(SOURCES
  LagrangeInterpolator.cc
  NewtonInterpolator.cc
)

uni_lib(LibInterpolation interpolation)
target_link_libraries(LibInterpolation PRIVATE LibMath)
