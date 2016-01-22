# vcl/tests/CMakeLists.txt

# Avoid adding more test executables here, in order to keep the number
# of projects small. (This is a concern for IDEs like Visual Studio.)
# Try to follow one of the other tests, like test_algorithm.cxx,
# and add the test into test_driver.cxx.

ADD_EXECUTABLE( vcl_test_all
 # Driver
     test_driver.cxx
 # The actual tests
     test_algorithm.cxx
     test_cctype.cxx
     test_cmath.cxx
     test_compiler.cxx
     test_complex.cxx
     test_deque.cxx
     test_exception.cxx
     test_fstream.cxx
     test_iostream.cxx
     test_iterator.cxx
     test_list.cxx
     test_limits.cxx
     test_map.cxx
     test_memory.cxx
     test_multimap.cxx
     test_new.cxx
     test_set.cxx
     test_stlfwd.cxx
     test_string.cxx
     test_sstream.cxx
     test_vector.cxx
     test_cstdio.cxx
     test_preprocessor.cxx
     test_atomic_count.cxx
     test_typename.cxx
)
TARGET_LINK_LIBRARIES( vcl_test_all vcl )

ADD_TEST( NAME vcl_test_algorithm COMMAND $<TARGET_FILE:vcl_test_all> test_algorithm )
ADD_TEST( NAME vcl_test_cctype COMMAND $<TARGET_FILE:vcl_test_all> test_cctype )
ADD_TEST( NAME vcl_test_cmath COMMAND $<TARGET_FILE:vcl_test_all> test_cmath )
ADD_TEST( NAME vcl_test_compiler COMMAND $<TARGET_FILE:vcl_test_all> test_compiler )
ADD_TEST( NAME vcl_test_complex COMMAND $<TARGET_FILE:vcl_test_all> test_complex )
ADD_TEST( NAME vcl_test_deque COMMAND $<TARGET_FILE:vcl_test_all> test_deque )
ADD_TEST( NAME vcl_test_exception COMMAND $<TARGET_FILE:vcl_test_all> test_exception )
ADD_TEST( NAME vcl_test_fstream COMMAND $<TARGET_FILE:vcl_test_all> test_fstream )
ADD_TEST( NAME vcl_test_iostream COMMAND $<TARGET_FILE:vcl_test_all> test_iostream )
ADD_TEST( NAME vcl_test_iterator COMMAND $<TARGET_FILE:vcl_test_all> test_iterator )
ADD_TEST( NAME vcl_test_list COMMAND $<TARGET_FILE:vcl_test_all> test_list )
ADD_TEST( NAME vcl_test_limits COMMAND $<TARGET_FILE:vcl_test_all> test_limits )
ADD_TEST( NAME vcl_test_map COMMAND $<TARGET_FILE:vcl_test_all> test_map )
ADD_TEST( NAME vcl_test_memory COMMAND $<TARGET_FILE:vcl_test_all> test_memory )
ADD_TEST( NAME vcl_test_multimap COMMAND $<TARGET_FILE:vcl_test_all> test_multimap )
ADD_TEST( NAME vcl_test_new COMMAND $<TARGET_FILE:vcl_test_all> test_new )
ADD_TEST( NAME vcl_test_set COMMAND $<TARGET_FILE:vcl_test_all> test_set )
ADD_TEST( NAME vcl_test_string COMMAND $<TARGET_FILE:vcl_test_all> test_string )
ADD_TEST( NAME vcl_test_sstream COMMAND $<TARGET_FILE:vcl_test_all> test_sstream )
ADD_TEST( NAME vcl_test_vector COMMAND $<TARGET_FILE:vcl_test_all> test_vector )
ADD_TEST( NAME vcl_test_cstdio COMMAND $<TARGET_FILE:vcl_test_all> test_cstdio ${CMAKE_CURRENT_SOURCE_DIR}/test_cstdio.txt )
ADD_TEST( NAME vcl_test_preprocessor COMMAND $<TARGET_FILE:vcl_test_all> test_preprocessor )
ADD_TEST( NAME vcl_test_atomic_count COMMAND $<TARGET_FILE:vcl_test_all> test_atomic_count )
# Only a compiler test:
#ADD_TEST( NAME vcl_test_stlfwd COMMAND $<TARGET_FILE:vcl_test_all> test_stlfwd )
#ADD_TEST( NAME vcl_test_typename COMMAND $<TARGET_FILE:vcl_test_all> test_typename )

ADD_EXECUTABLE( vcl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vcl_test_include vcl )
