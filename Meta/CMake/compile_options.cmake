add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wimplicit-fallthrough)
add_compile_options(-Wmisleading-indentation)
add_compile_options(-Wmissing-declarations)
add_compile_options(-Wundef)
add_compile_options(-Wunused)

# FIXME: enable iff the compiler supports this flag
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fconcepts-diagnostics-depth=4)
endif()
