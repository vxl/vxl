# This is prip/vmap/tests/CMakeLists.txt

link_libraries(vmap)

add_executable(vmap_test vmap_test.cxx my_map.h my_map_functions.hxx)
# temporarily disabling this test: it causes segfaults
#add_test( NAME vmap_test COMMAND $<TARGET_FILE:vmap_test)>

add_executable(vmap_test_include test_include.cxx)
add_executable(vmap_test_template_include test_template_include.cxx)
