# brl/bbas/bgui/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR})

add_executable(bgui_test_all
  test_driver.cxx
  test_histogram.cxx
)
target_link_libraries(bgui_test_all ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl)

add_test( NAME bgui_test_histogram COMMAND $<TARGET_FILE:bgui_test_all> test_histogram ${CMAKE_CURRENT_SOURCE_DIR}/18_al05_1770_0365.tif )

add_executable(bgui_test_include test_include.cxx)
target_link_libraries(bgui_test_include bgui)
