# ./vcl/tests/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

# Avoid adding more test executables here, in order to keep the number
# of projects small. (This is concern for IDEs like Visual Studio.)
# Try to follow one of the other tests, like test_vcl_algorithm.cxx,
# and add the test into vcl_tests.cpp.

ADD_EXECUTABLE( vcl_tests vcl_tests.cpp)

ADD_TEST( vcl_tests vcl_tests)
