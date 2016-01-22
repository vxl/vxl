# gel/gevd/tests/CMakeLists.txt

ADD_EXECUTABLE(gevd_test_all
               test_driver.cxx
               test_gevd_param_mixin.cxx
               test_gevd_memory_mixin.cxx
               test_gevd_bufferxy.cxx
               test_gevd_noise.cxx
               test_gevd_float_operators.cxx
              )
TARGET_LINK_LIBRARIES( gevd_test_all gevd testlib vil vnl vpl )

ADD_TEST(gevd_test_param_mixin     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gevd_test_all test_gevd_param_mixin)
ADD_TEST(gevd_test_memory_mixin    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gevd_test_all test_gevd_memory_mixin)
ADD_TEST(gevd_test_bufferxy        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gevd_test_all test_gevd_bufferxy)
ADD_TEST(gevd_test_noise           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gevd_test_all test_gevd_noise)
ADD_TEST(gevd_test_float_operators ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gevd_test_all test_gevd_float_operators)

ADD_EXECUTABLE( gevd_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( gevd_test_include gevd )
