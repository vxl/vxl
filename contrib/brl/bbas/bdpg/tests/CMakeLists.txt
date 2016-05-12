# brl/bbas/bdpg/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR})

add_executable(bdpg_test_all test_driver.cxx test_array_dynamic_prg.cxx)

target_link_libraries(bdpg_test_all bdpg ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib)

add_test( NAME bdpg_test_array_dynamic_prg COMMAND $<TARGET_FILE:bdpg_test_all> test_array_dynamic_prg)

add_executable(bdpg_test_include test_include.cxx)
target_link_libraries(bdpg_test_include bdpg)
