INCLUDE( ${allvxl_SOURCE_DIR}/conversions/geometry/CMakeListsLink.txt )

IF(BUILD_GEL)
 IF(BUILD_OXL)
  ADD_EXECUTABLE( conversions_test_osl_to_vdgl test_osl_to_vdgl.cxx )
  ADD_TEST( conversions_test_osl_to_vdgl conversions_test_osl_to_vdgl )
 ENDIF(BUILD_OXL)
ENDIF(BUILD_GEL)

IF(HAS_SPATIALOBJECTS)
  ADD_EXECUTABLE( conversions_test_DigitalCurve_to_vdgl test_DigitalCurve_to_vdgl.cxx )
  ADD_TEST( conversions_test_DigitalCurve_to_vdgl conversions_test_DigitalCurve_to_vdgl )
ENDIF(HAS_SPATIALOBJECTS)
