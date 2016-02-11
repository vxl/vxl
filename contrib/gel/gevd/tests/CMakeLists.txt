# gel/gevd/tests/CMakeLists.txt

add_executable(gevd_test_all
               test_driver.cxx
               test_gevd_param_mixin.cxx
               test_gevd_memory_mixin.cxx
               test_gevd_bufferxy.cxx
               test_gevd_noise.cxx
               test_gevd_float_operators.cxx
              )
target_link_libraries( gevd_test_all gevd ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpl )

add_test( NAME gevd_test_param_mixin COMMAND $<TARGET_FILE:gevd_test_all> test_gevd_param_mixin)
add_test( NAME gevd_test_memory_mixin COMMAND $<TARGET_FILE:gevd_test_all> test_gevd_memory_mixin)
add_test( NAME gevd_test_bufferxy COMMAND $<TARGET_FILE:gevd_test_all> test_gevd_bufferxy)
add_test( NAME gevd_test_noise COMMAND $<TARGET_FILE:gevd_test_all> test_gevd_noise)
add_test( NAME gevd_test_float_operators COMMAND $<TARGET_FILE:gevd_test_all> test_gevd_float_operators)

add_executable( gevd_test_include test_include.cxx )
target_link_libraries( gevd_test_include gevd )
