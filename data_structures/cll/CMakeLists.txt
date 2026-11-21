add_executable( cll 
    cll.cpp 
    main_cll.cpp 
)
install(TARGETS cll DESTINATION "bin/data_structures")

# Regression test for size accounting (R22-T040); separate target so its
# main() does not collide with main_cll.cpp.
add_executable( cll_test
    cll.cpp
    cll_test.cpp
)
