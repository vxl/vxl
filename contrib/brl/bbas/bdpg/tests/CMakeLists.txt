# brl/bbas/bdpg/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

ADD_EXECUTABLE(bdpg_test_all test_driver.cxx test_array_dynamic_prg.cxx)

TARGET_LINK_LIBRARIES(bdpg_test_all bdpg testlib)

ADD_TEST(bdpg_test_array_dynamic_prg bdpg_test_all test_array_dynamic_prg)
