# This is brl/bbas/bgeo/tests/CMakeLists.txt

ADD_EXECUTABLE ( bgeo_test_all
  test_driver.cxx
  test_lvcs.cxx
  test_sun_pos.cxx
)
TARGET_LINK_LIBRARIES( bgeo_test_all bgeo vgl testlib )

ADD_TEST( bgeo_test_lvcs   ${EXECUTABLE_OUTPUT_PATH}/bgeo_test_all test_lvcs )
ADD_TEST( bgeo_test_sun_pos   ${EXECUTABLE_OUTPUT_PATH}/bgeo_test_all test_sun_pos )

ADD_EXECUTABLE(bgeo_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bgeo_test_include bgeo)
