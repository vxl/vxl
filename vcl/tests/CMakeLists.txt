#
# ./vcl/tests/CMakeLists.txt
#
#   ian.m.scott@stud.man.ac.uk
#

LINK_DIRECTORIES(
${allvxl_BINARY_DIR}/vcl
)

LINK_LIBRARIES(
vcl
)

IF(UNIX)
  LINK_LIBRARIES( m )
ENDIF(UNIX)


ADD_EXECUTABLE(
vcl_tests vcl_tests.cpp
)

ADD_EXECUTABLE(
test_vcl_algorithm test_vcl_algorithm
)

ADD_EXECUTABLE(
test_vcl_cmath test_vcl_cmath
)

ADD_EXECUTABLE(
test_vcl_compiler test_vcl_compiler
)

ADD_EXECUTABLE(
test_vcl_complex test_vcl_complex
)

ADD_EXECUTABLE(
test_vcl_deque test_vcl_deque
)

ADD_EXECUTABLE(
test_vcl_exception test_vcl_exception
)

ADD_EXECUTABLE(
test_vcl_fstream test_vcl_fstream
)

ADD_EXECUTABLE(
test_vcl_headers test_vcl_headers
)

ADD_EXECUTABLE(
test_vcl_iostream test_vcl_iostream
)

ADD_EXECUTABLE(
test_vcl_iterator test_vcl_iterator
)

ADD_EXECUTABLE(
test_vcl_list test_vcl_list
)

ADD_EXECUTABLE(
test_vcl_map test_vcl_map
)

ADD_EXECUTABLE(
test_vcl_multimap test_vcl_multimap
)

ADD_EXECUTABLE(
test_vcl_new test_vcl_new
)

ADD_EXECUTABLE(
test_vcl_rel_ops test_vcl_rel_ops
)

ADD_EXECUTABLE(
test_vcl_set test_vcl_set
)

ADD_EXECUTABLE(
test_vcl_stlfwd test_vcl_stlfwd
)

ADD_EXECUTABLE(
test_vcl_string test_vcl_string
)

ADD_EXECUTABLE(
test_vcl_vector test_vcl_vector
)
