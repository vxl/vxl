link_libraries( vgl_algo vnl_algo vbl testlib )

if(BUILD_GEL)
  include_directories(${GEL_INCLUDE_DIR})
  link_libraries( vdgl )
endif()
if(BUILD_OXL)
  include_directories(${OXL_INCLUDE_DIR})
  link_libraries( osl )
endif()

if(BUILD_GEL)
  add_definitions(-DHAS_VDGL=1)
  if(BUILD_OXL)
    add_definitions(-DHAS_OSL=1)
  endif()
endif()

set(HAS_SPATIALOBJECTS "NO")
if(EXISTS ${vxl_SOURCE_DIR}/SpatialObjects/CMakeLists.txt )
  include_directories(${vxl_SOURCE_DIR}/COOL)
  include_directories(${vxl_SOURCE_DIR}/GeneralUtility)
  include_directories(${vxl_SOURCE_DIR}/SpatialObjects)
  include_directories(${vxl_SOURCE_DIR}/Numerics)
  link_libraries( DigitalGeometry CompGeom Topology Groups Geometry SpatialBasics Basics Solver Numerics )
set(HAS_SPATIALOBJECTS "YES")
endif()

if(HAS_SPATIALOBJECTS)
  add_definitions(-DHAS_SPATIALOBJECTS=1)
else()
  add_definitions(-DHAS_NO_SPATIALOBJECTS=1)
endif()

add_executable( conversions_geometry_test_all
  test_driver.cxx
  test_osl_to_vdgl.cxx
  test_DigitalCurve_to_vdgl.cxx
)

add_test( NAME conversions_test_osl_to_vdgl COMMAND $<TARGET_FILE:conversions_geometry_test_all> test_osl_to_vdgl )
add_test( NAME conversions_test_DigitalCurve_to_vdgl COMMAND $<TARGET_FILE:conversions_geometry_test_all> test_DigitalCurve_to_vdgl )

add_executable( conversions_geometry_test_include test_include.cxx )