# vcl/tests/CMakeLists.txt

# Avoid adding more test executables here, in order to keep the number
# of projects small. (This is a concern for IDEs like Visual Studio.)
# Try to follow one of the other tests, like test_algorithm.cxx,
# and add the test into test_driver.cxx.

ADD_EXECUTABLE( vcl_test_driver
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
     test_headers.cxx
     test_iostream.cxx
     test_iterator.cxx
     test_list.cxx
     test_map.cxx
     test_multimap.cxx
     test_new.cxx
     test_set.cxx
     test_stlfwd.cxx
     test_string.cxx
     test_vector.cxx
     test_cstdio.cxx
)
TARGET_LINK_LIBRARIES( vcl_test_driver vcl )

ADD_TEST( vcl_test_algorithm   vcl_test_driver test_algorithm )
ADD_TEST( vcl_test_cctype      vcl_test_driver test_cctype )
ADD_TEST( vcl_test_cmath       vcl_test_driver test_cmath )
ADD_TEST( vcl_test_compiler    vcl_test_driver test_compiler )
ADD_TEST( vcl_test_complex     vcl_test_driver test_complex )
ADD_TEST( vcl_test_deque       vcl_test_driver test_deque )
ADD_TEST( vcl_test_exception   vcl_test_driver test_exception )
ADD_TEST( vcl_test_fstream     vcl_test_driver test_fstream )
ADD_TEST( vcl_test_iostream    vcl_test_driver test_iostream )
ADD_TEST( vcl_test_iterator    vcl_test_driver test_iterator )
ADD_TEST( vcl_test_list        vcl_test_driver test_list )
ADD_TEST( vcl_test_map         vcl_test_driver test_map )
ADD_TEST( vcl_test_multimap    vcl_test_driver test_multimap )
ADD_TEST( vcl_test_new         vcl_test_driver test_new )
ADD_TEST( vcl_test_set         vcl_test_driver test_set )
ADD_TEST( vcl_test_string      vcl_test_driver test_string )
ADD_TEST( vcl_test_vector      vcl_test_driver test_vector )
ADD_TEST( vcl_test_cstdio      vcl_test_driver test_cstdio ${allvxl_SOURCE_DIR}/vcl/tests/test_cstdio.txt )
# Only a compiler test:
#ADD_TEST( vcl_test_headers     vcl_test_driver test_headers )
#ADD_TEST( vcl_test_stlfwd      vcl_test_driver test_stlfwd )
# Deprecated:
#ADD_TEST( vcl_test_rel_ops     vcl_test_driver test_rel_ops )
