# ./vcl/tests/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

# Avoid adding more test executables here, in order to keep the number
# of projects small. (This is concern for IDEs like Visual Studio.)
# Try to follow one of the other tests, like test_vcl_algorithm.cxx,
# and add the test into vcl_tests.cpp.

ADD_EXECUTABLE( vcl_tests vcl_tests.cpp)

# Rel_ops conflicts with vector::iterator under GCC 3.0, so it can't
# be put into the main test.
ADD_EXECUTABLE( vcl_test_rel_ops test_vcl_rel_ops.cxx)

ADD_TEST( vcl_tests vcl_tests)
ADD_TEST( vcl_test_rel_ops vcl_test_rel_ops)
