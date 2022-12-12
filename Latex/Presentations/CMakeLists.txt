add_subdirectory (DiscreteRobustness)

add_custom_target (Presentations)
add_dependencies (Presentations
  DiscreteRobustness
)
