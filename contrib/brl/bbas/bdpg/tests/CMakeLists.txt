# brl/bbas/bdpg/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})

ADD_EXECUTABLE(bdpg_test_all test_driver.cxx test_array_dynamic_prg.cxx)

TARGET_LINK_LIBRARIES(bdpg_test_all bdpg vnl testlib)

ADD_TEST( NAME bdpg_test_array_dynamic_prg COMMAND $<TARGET_FILE:bdpg_test_all> test_array_dynamic_prg)

ADD_EXECUTABLE(bdpg_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bdpg_test_include bdpg)
