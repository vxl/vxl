INCLUDE( ${allvxl_SOURCE_DIR}/conversions/geometry/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

IF(HAS_SPATIALOBJECTS)
ELSE(HAS_SPATIALOBJECTS)
  ADD_DEFINITIONS(-DHAS_NO_SPATIALOBJECTS=1)
ENDIF(HAS_SPATIALOBJECTS)

ADD_EXECUTABLE( conversions_geometry_test_driver
  test_driver.cxx

  test_osl_to_vdgl.cxx
  test_DigitalCurve_to_vdgl.cxx
)

ADD_TEST( conversions_test_osl_to_vdgl conversions_geometry_test_driver test_osl_to_vdgl )
ADD_TEST( conversions_test_DigitalCurve_to_vdgl conversions_geometry_test_driver test_DigitalCurve_to_vdgl )
