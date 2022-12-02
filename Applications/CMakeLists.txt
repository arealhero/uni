add_subdirectory(Interpolation)

add_custom_target (Applications)
add_dependencies (Applications Interpolation)
