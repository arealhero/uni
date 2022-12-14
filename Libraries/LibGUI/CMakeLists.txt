find_package(Python3 COMPONENTS Interpreter Development NumPy)

set(SOURCES
  Plot.cc
)

uni_lib (LibGUI gui)
target_include_directories (LibGUI SYSTEM PRIVATE ${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})
target_link_libraries (LibGUI PRIVATE LibMath ${Python3_LIBRARIES})
