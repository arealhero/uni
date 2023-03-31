add_subdirectory(Approximation)
add_subdirectory(Interpolation)
add_subdirectory(Integration)
add_subdirectory(JSP)

add_custom_target (Applications)
add_dependencies (Applications
  Approximation
  Interpolation
  Integration
  JSP
)
