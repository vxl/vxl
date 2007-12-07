# brl/bbas/bdpg/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})

ADD_EXECUTABLE(bdpg_test_all test_driver.cxx test_array_dynamic_prg.cxx)

TARGET_LINK_LIBRARIES(bdpg_test_all bdpg testlib)

ADD_TEST(bdpg_test_array_dynamic_prg ${EXECUTABLE_OUTPUT_PATH}/bdpg_test_all test_array_dynamic_prg)

ADD_EXECUTABLE(bdpg_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bdpg_test_include bdpg)
