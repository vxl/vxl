# gel/gevd/tests/CMakeLists.txt

LINK_LIBRARIES( gevd testlib vpl )

ADD_EXECUTABLE(test_gevd_param_mixin  test_gevd_param_mixin.cxx)
ADD_TEST(      test_gevd_param_mixin  test_gevd_param_mixin)
ADD_EXECUTABLE(test_gevd_memory_mixin test_gevd_memory_mixin.cxx)
ADD_TEST(      test_gevd_memory_mixin test_gevd_memory_mixin)
ADD_EXECUTABLE(test_gevd_bufferxy     test_gevd_bufferxy.cxx)
ADD_TEST(      test_gevd_bufferxy     test_gevd_bufferxy)
ADD_EXECUTABLE(test_gevd_float_operators test_gevd_float_operators.cxx)
ADD_TEST(      test_gevd_float_operators test_gevd_float_operators)
