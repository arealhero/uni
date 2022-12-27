add_subdirectory (DynamicRegulator)

add_custom_target (Documents)
add_dependencies (Documents
  DynamicRegulator
)
