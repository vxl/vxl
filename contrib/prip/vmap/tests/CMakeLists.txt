# This is contrib/prip/tests/CMakeLists.txt

LINK_LIBRARIES(vmap)

ADD_EXECUTABLE( vmap_test vmap_test.cxx my_map.h my_map_functions.txx)

ADD_EXECUTABLE( vmap_test_include test_include.cxx )
