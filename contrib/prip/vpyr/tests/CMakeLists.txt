# This is contrib/prip/tests/CMakeLists.txt

LINK_LIBRARIES(vpyr)

ADD_EXECUTABLE( vpyr_test vpyr_test.cxx my_pyramid.cxx my_pyramid.h my_pyramid_functions.txx )
ADD_EXECUTABLE( vpyr_test2 vpyr_test2.cxx my_pyramid.cxx my_pyramid.h my_pyramid_functions.txx my_top_level.h my_top_level.cxx )

ADD_EXECUTABLE( vpyr_test_include test_include.cxx )
