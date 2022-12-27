add_subdirectory (Documents)
add_subdirectory (Presentations)

add_custom_target (Latex)
add_dependencies (Latex
  Documents
  Presentations
)
