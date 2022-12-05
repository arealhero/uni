set(SOURCES
    main.cc
)

uni_bin(Approximation)
target_link_libraries(Approximation PRIVATE LibMath LibGUI LibInterpolation LibApproximation)
