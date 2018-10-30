link_libraries( ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib )

if(VXL_BUILD_GEL)
  include_directories(${GEL_INCLUDE_DIR})
  link_libraries( vdgl )
endif()
if(VXL_BUILD_OXL)
  include_directories(${OXL_INCLUDE_DIR})
  link_libraries( osl )
endif()

if(VXL_BUILD_GEL)
  add_definitions(-DHAS_VDGL=1)
  if(VXL_BUILD_OXL)
    add_definitions(-DHAS_OSL=1)
  endif()
endif()

set(HAS_SPATIALOBJECTS "NO")
if(EXISTS ${CMAKE_SOURCE_DIR}/SpatialObjects/CMakeLists.txt )
  include_directories(${CMAKE_SOURCE_DIR}/COOL)
  include_directories(${CMAKE_SOURCE_DIR}/GeneralUtility)
  include_directories(${CMAKE_SOURCE_DIR}/SpatialObjects)
  include_directories(${CMAKE_SOURCE_DIR}/Numerics)
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
