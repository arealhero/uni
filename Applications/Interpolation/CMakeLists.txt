set(SOURCES
    main.cc
)

uni_bin(Interpolation)
target_link_libraries(Interpolation PRIVATE LibMath LibInterpolation)
