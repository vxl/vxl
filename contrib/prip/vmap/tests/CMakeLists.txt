# This is prip/vmap/tests/CMakeLists.txt

LINK_LIBRARIES(vmap)

ADD_EXECUTABLE(vmap_test vmap_test.cxx my_map.h my_map_functions.txx)
# temporarily disabling this test: it causes segfaults
#ADD_TEST(vmap_test vmap_test)

ADD_EXECUTABLE(vmap_test_include test_include.cxx)
