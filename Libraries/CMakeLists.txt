add_subdirectory (LibInterpolation)
add_subdirectory (LibMath)
add_subdirectory (LibSLE)

add_custom_target (Libraries)
add_dependencies (Libraries LibInterpolation)
add_dependencies (Libraries LibMath)
