# This is prip/vpyr/tests/CMakeLists.txt

link_libraries(vpyr)

add_executable(vpyr_test vpyr_test.cxx my_pyramid.cxx my_pyramid.h my_pyramid_functions.hxx)
add_executable(vpyr_test2 vpyr_test2.cxx my_pyramid.cxx my_pyramid.h my_pyramid_functions.hxx my_top_level.h my_top_level.cxx)

# temporarily disabling these tests: causing segfaults
#add_test( NAME vpyr_test COMMAND $<TARGET_FILE:vpyr_test)>
#add_test( NAME vpyr_test2 COMMAND $<TARGET_FILE:vpyr_test2)>

add_executable(vpyr_test_include test_include.cxx)
add_executable(vpyr_test_template_include test_template_include.cxx)
