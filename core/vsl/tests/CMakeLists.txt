#
# ./vxl/vsl/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vsl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )

ADD_EXECUTABLE( test_indent test_indent.cxx )
ADD_EXECUTABLE( test_vsl_io test_vsl_io.cxx )
ADD_EXECUTABLE( test_binary_io test_binary_io.cxx )

ADD_EXECUTABLE( test_arbitrary_length_int_conversion test_arbitrary_length_int_conversion.cxx )
ADD_EXECUTABLE( test_clipon_polymorphic_io test_clipon_polymorphic_io.cxx )
ADD_EXECUTABLE( test_complex_io test_complex_io.cxx )
ADD_EXECUTABLE( test_deque_io test_deque_io.cxx )
ADD_EXECUTABLE( test_list_io test_list_io.cxx )
ADD_EXECUTABLE( test_map_io test_map_io.cxx )
ADD_EXECUTABLE( test_polymorphic_io test_polymorphic_io.cxx )
ADD_EXECUTABLE( test_set_io test_set_io.cxx )
ADD_EXECUTABLE( test_stack_io test_stack_io.cxx )
ADD_EXECUTABLE( test_string_io test_string_io.cxx )
ADD_EXECUTABLE( test_vector_io test_vector_io.cxx )


ADD_TEST( vsl_test_indent test_indent )
ADD_TEST( vsl_test_vsl_io test_vsl_io )
ADD_TEST( vsl_test_binary_io test_binary_io )

ADD_TEST( vsl_test_arbitrary_length_int_conversion test_arbitrary_length_int_conversion )
ADD_TEST( vsl_test_clipon_polymorphic_io test_clipon_polymorphic_io )
ADD_TEST( vsl_test_complex_io test_complex_io )
ADD_TEST( vsl_test_deque_io test_deque_io )
ADD_TEST( vsl_test_list_io test_list_io )
ADD_TEST( vsl_test_map_io test_map_io )
ADD_TEST( vsl_test_polymorphic_io test_polymorphic_io )
ADD_TEST( vsl_test_set_io test_set_io )
ADD_TEST( vsl_test_stack_io test_stack_io )
ADD_TEST( vsl_test_string_io test_string_io )
ADD_TEST( vsl_test_vector_io test_vector_io )
