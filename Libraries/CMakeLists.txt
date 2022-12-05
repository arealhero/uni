add_subdirectory (LibApproximation)
add_subdirectory (LibInterpolation)
add_subdirectory (LibMath)
add_subdirectory (LibGUI)
add_subdirectory (LibSLE)

add_custom_target (Libraries)
add_dependencies (Libraries
  LibApproximation
  LibInterpolation
  LibMath
  LibGUI
)
