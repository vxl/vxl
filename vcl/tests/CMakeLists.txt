# ./vcl/tests/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

# Avoid adding more test executables here, in order to keep the number
# of projects small. (This is concern for IDEs like Visual Studio.)
# Try to follow one of the other tests, like test_vcl_algorithm.cxx,
# and add the test into vcl_tests.cxx.

ADD_EXECUTABLE( vcl_tests
 # Driver
     vcl_tests.cxx
 # The actual tests
     test_vcl_algorithm.cxx
     test_vcl_cctype.cxx
     test_vcl_cmath.cxx
     test_vcl_compiler.cxx
     test_vcl_complex.cxx
     test_vcl_deque.cxx
     test_vcl_exception.cxx
     test_vcl_fstream.cxx
     test_vcl_headers.cxx
     test_vcl_iostream.cxx
     test_vcl_iterator.cxx
     test_vcl_list.cxx
     test_vcl_map.cxx
     test_vcl_multimap.cxx
     test_vcl_new.cxx
     test_vcl_rel_ops.cxx
     test_vcl_set.cxx
     test_vcl_stlfwd.cxx
     test_vcl_string.cxx
     test_vcl_vector.cxx
)

ADD_TEST( vcl_tests vcl_tests)
