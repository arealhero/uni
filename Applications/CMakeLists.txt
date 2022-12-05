add_subdirectory(Approximation)
add_subdirectory(Interpolation)

add_custom_target (Applications)
add_dependencies (Applications
  Approximation
  Interpolation
)
