LINK_LIBRARIES( vgl_algo vnl_algo vbl testlib )

IF(BUILD_GEL)
  INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
  LINK_LIBRARIES( vdgl )
ENDIF(BUILD_GEL)
IF(BUILD_OXL)
  INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
  LINK_LIBRARIES( osl )
ENDIF(BUILD_OXL)

IF(BUILD_GEL)
  ADD_DEFINITIONS(-DHAS_VDGL=1)
  IF(BUILD_OXL)
    ADD_DEFINITIONS(-DHAS_OSL=1)
  ENDIF(BUILD_OXL)
ENDIF(BUILD_GEL)

SET(HAS_SPATIALOBJECTS "NO")
IF(EXISTS ${vxl_SOURCE_DIR}/SpatialObjects/CMakeLists.txt )
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/COOL)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/GeneralUtility)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/SpatialObjects)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/Numerics)
  LINK_LIBRARIES( DigitalGeometry CompGeom Topology Groups Geometry SpatialBasics Basics Solver Numerics )
SET(HAS_SPATIALOBJECTS "YES")
ENDIF(EXISTS ${vxl_SOURCE_DIR}/SpatialObjects/CMakeLists.txt )

IF(HAS_SPATIALOBJECTS)
  ADD_DEFINITIONS(-DHAS_SPATIALOBJECTS=1)
ELSE(HAS_SPATIALOBJECTS)
  ADD_DEFINITIONS(-DHAS_NO_SPATIALOBJECTS=1)
ENDIF(HAS_SPATIALOBJECTS)

ADD_EXECUTABLE( conversions_geometry_test_all
  test_driver.cxx
  test_osl_to_vdgl.cxx
  test_DigitalCurve_to_vdgl.cxx
)

ADD_TEST( conversions_test_osl_to_vdgl conversions_geometry_test_all test_osl_to_vdgl )
ADD_TEST( conversions_test_DigitalCurve_to_vdgl conversions_geometry_test_all test_DigitalCurve_to_vdgl )

ADD_EXECUTABLE( conversions_geometry_test_include test_include.cxx )
