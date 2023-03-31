set(SOURCES
    main.cc
)

uni_bin(Integration)
target_link_libraries(Integration PRIVATE LibGUI)

