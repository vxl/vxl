# This is core/vsl/tests/CMakeLists.txt

LINK_LIBRARIES( vsl testlib vpl )

ADD_EXECUTABLE( vsl_test_all
  test_driver.cxx
  test_indent.cxx
  test_binary_io.cxx
  test_arbitrary_length_int_conversion.cxx
  test_clipon_polymorphic_io.cxx
  test_complex_io.cxx
  test_deque_io.cxx
  test_list_io.cxx
  test_map_io.cxx
  test_polymorphic_io.cxx
  test_set_io.cxx
  test_stack_io.cxx
  test_string_io.cxx
  test_vector_io.cxx
)
 
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_map_io.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_TEST(vsl_test_indent                vsl_test_all test_indent)
ADD_TEST(vsl_test_binary_io             vsl_test_all test_binary_io)

ADD_TEST(vsl_test_arbitrary_length_int_conversion vsl_test_all
                                           test_arbitrary_length_int_conversion)
ADD_TEST(vsl_test_clipon_polymorphic_io vsl_test_all test_clipon_polymorphic_io)
ADD_TEST(vsl_test_complex_io            vsl_test_all test_complex_io)
ADD_TEST(vsl_test_deque_io              vsl_test_all test_deque_io)
ADD_TEST(vsl_test_list_io               vsl_test_all test_list_io)
ADD_TEST(vsl_test_map_io                vsl_test_all test_map_io)
ADD_TEST(vsl_test_polymorphic_io        vsl_test_all test_polymorphic_io)
ADD_TEST(vsl_test_set_io                vsl_test_all test_set_io)
ADD_TEST(vsl_test_stack_io              vsl_test_all test_stack_io)
ADD_TEST(vsl_test_string_io             vsl_test_all test_string_io)
ADD_TEST(vsl_test_vector_io             vsl_test_all test_vector_io)

ADD_EXECUTABLE( vsl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsl_test_include vsl )
