# brl/bpro/bprb/tests/CMakeLists.txt
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/params.xml
                ${CMAKE_CURRENT_BINARY_DIR}/params.xml)
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)

  add_executable( bprb_test_all
   test_driver.cxx
   test_process.cxx
   test_process_params.cxx
   bprb_test_process.h bprb_test_process.cxx
  )
  target_link_libraries( bprb_test_all bprb ${VXL_LIB_PREFIX}testlib expat expatpp)

  add_test( NAME bprb_test_process COMMAND $<TARGET_FILE:bprb_test_all> test_process )
  add_test( NAME bprb_test_process_params COMMAND $<TARGET_FILE:bprb_test_all> test_process_params )
 endif()
endif()

add_executable(bprb_test_include test_include.cxx)
target_link_libraries(bprb_test_include bprb)
add_executable(bprb_test_template_include test_template_include.cxx)
target_link_libraries(bprb_test_template_include bprb)
