add_subdirectory (DiscreteRobustness)
add_subdirectory (DynamicRegulator)

add_custom_target (Presentations)
add_dependencies (Presentations
  DiscreteRobustness
  DynamicRegulatorPresentation
)
