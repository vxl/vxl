# This is core/tests/CMakeLists.txt

ADD_EXECUTABLE( vxl_test_all
  test_driver.cxx
  test_config.cxx
)

TARGET_LINK_LIBRARIES( vxl_test_all testlib vcl )

ADD_EXECUTABLE( vxl_echo
  vxl_echo.cxx
)


ADD_TEST( vxl_test_config vxl_test_all test_config )

ADD_TEST( cmake_config vxl_echo
            MAKECOMMAND: ${MAKECOMMAND} nl
            CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS} nl
            CMAKE_C_FLAGS: ${CMAKE_C_FLAGS} nl
            BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS} nl
            CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS} nl
            CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS}
            CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS} nl
            )