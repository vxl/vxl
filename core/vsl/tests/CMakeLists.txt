# This is core/vsl/tests/CMakeLists.txt

link_libraries( ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl )

add_executable( vsl_test_all
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
  test_block_rle_io.cxx
)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(test_map_io.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

add_test( NAME vsl_test_indent COMMAND $<TARGET_FILE:vsl_test_all> test_indent)
add_test( NAME vsl_test_binary_io COMMAND $<TARGET_FILE:vsl_test_all> test_binary_io)

add_test( NAME vsl_test_arbitrary_length_int_conversion COMMAND $<TARGET_FILE:vsl_test_all>
                                           test_arbitrary_length_int_conversion)
add_test( NAME vsl_test_clipon_polymorphic_io COMMAND $<TARGET_FILE:vsl_test_all> test_clipon_polymorphic_io)
add_test( NAME vsl_test_complex_io COMMAND $<TARGET_FILE:vsl_test_all> test_complex_io)
add_test( NAME vsl_test_deque_io COMMAND $<TARGET_FILE:vsl_test_all> test_deque_io)
add_test( NAME vsl_test_list_io COMMAND $<TARGET_FILE:vsl_test_all> test_list_io)
add_test( NAME vsl_test_map_io COMMAND $<TARGET_FILE:vsl_test_all> test_map_io)
add_test( NAME vsl_test_polymorphic_io COMMAND $<TARGET_FILE:vsl_test_all> test_polymorphic_io)
add_test( NAME vsl_test_set_io COMMAND $<TARGET_FILE:vsl_test_all> test_set_io)
add_test( NAME vsl_test_stack_io COMMAND $<TARGET_FILE:vsl_test_all> test_stack_io)
add_test( NAME vsl_test_string_io COMMAND $<TARGET_FILE:vsl_test_all> test_string_io)
add_test( NAME vsl_test_vector_io COMMAND $<TARGET_FILE:vsl_test_all> test_vector_io)
add_test( NAME vsl_test_block_rle_io COMMAND $<TARGET_FILE:vsl_test_all> test_block_rle_io)

# Don't add test_vlarge_block_io to the automatic list. It does nasty things
# to memory which can result in wierd error messages, system lockup, and other
# unpleasant behaviour on some platforms
# The following manual tests have been tried.
# x86, Win2k, MSVC7.1 Release mode - test passes
# x86, Win2k, MSVC7.1 Debug mode - OS complains that you are messing with the
#            system dlls.
# x86_64, Win7, MSVC9.0 Release mode - test passes.
# x86_64, Win7, MSVC9.0 Debug mode - test passes if you explicitly cancel OS low memory warnings.
# x86_64, Linux 2.6, gcc 4.0.2, Linux VM doesn't limit the amount of memory -
#            so the test cannot simulate low memory conditions.
# x86_64, Linux 2.6, gcc 4.0.2 -fno-exceptions, ulimit -v 2000000, test passes
# x86_64, Linux 2.6, gcc 4.0.2, ulimit -v 2000000, test passes


add_executable( vsl_test_include test_include.cxx )
target_link_libraries( vsl_test_include ${VXL_LIB_PREFIX}vsl )
add_executable( vsl_test_template_include test_template_include.cxx )
target_link_libraries( vsl_test_template_include ${VXL_LIB_PREFIX}vsl )
