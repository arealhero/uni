add_subdirectory(Approximation)
add_subdirectory(Interpolation)
add_subdirectory(JSP)

add_custom_target (Applications)
add_dependencies (Applications
  Approximation
  Interpolation
  JSP
)
