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
  test_vlarge_block_io.cxx
)
 
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_map_io.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_TEST(vsl_test_indent                ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_indent)
ADD_TEST(vsl_test_binary_io             ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_binary_io)

ADD_TEST(vsl_test_arbitrary_length_int_conversion ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all
                                           test_arbitrary_length_int_conversion)
ADD_TEST(vsl_test_clipon_polymorphic_io ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_clipon_polymorphic_io)
ADD_TEST(vsl_test_complex_io            ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_complex_io)
ADD_TEST(vsl_test_deque_io              ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_deque_io)
ADD_TEST(vsl_test_list_io               ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_list_io)
ADD_TEST(vsl_test_map_io                ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_map_io)
ADD_TEST(vsl_test_polymorphic_io        ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_polymorphic_io)
ADD_TEST(vsl_test_set_io                ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_set_io)
ADD_TEST(vsl_test_stack_io              ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_stack_io)
ADD_TEST(vsl_test_string_io             ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_string_io)
ADD_TEST(vsl_test_vector_io             ${EXECUTABLE_OUTPUT_PATH}/vsl_test_all test_vector_io)

# Don't add test_vlarge_block_io to the automatic list. It does nasty things 
# to memory which can result in wierd error messages, system lockup, and other
# unpleasant behaviour on some platforms
# The following manual tests have been tried.
# x86, Win2k, MSVC7.1 Release mode - test passes
# x86, Win2k, MSVC7.1 Debug mode - OS complains that you are messing with the
#            system dlls.
# x86_64, Linux 2.6, gcc 4.0.2, Linux VM doesn't limit the amount of memory -
#            so the test cannot simulate low memory conditions.
# x86_64, Linux 2.6, gcc 4.0.2 -fno-exceptions, ulimit -v 2000000, test passes
# x86_64, Linux 2.6, gcc 4.0.2, ulimit -v 2000000, test passes


ADD_EXECUTABLE( vsl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsl_test_include vsl )
