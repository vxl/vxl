# gel/gevd/tests/CMakeLists.txt

LINK_LIBRARIES( gevd testlib vpl )

ADD_EXECUTABLE(gevd_test_all
               test_driver.cxx
               test_gevd_param_mixin.cxx
               test_gevd_memory_mixin.cxx
               test_gevd_bufferxy.cxx
               test_gevd_noise.cxx
               test_gevd_float_operators.cxx
              )
ADD_TEST(test_gevd_param_mixin     gevd_test_all test_gevd_param_mixin)
ADD_TEST(test_gevd_memory_mixin    gevd_test_all test_gevd_memory_mixin)
ADD_TEST(test_gevd_bufferxy        gevd_test_all test_gevd_bufferxy)
ADD_TEST(test_gevd_noise           gevd_test_all test_gevd_noise)
ADD_TEST(test_gevd_float_operators gevd_test_all test_gevd_float_operators)

ADD_EXECUTABLE( gevd_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( gevd_test_include gevd )
