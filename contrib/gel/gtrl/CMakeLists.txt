# gel/gtrl/CMakeLists.txt

doxygen_add_library(contrib/gel/gtrl
  DEPENDS core/vbl v3p/netlib
  PACKAGE contrib/gel
  DESCRIPTION "Polygon triangulation"
)

set(gtrl_sources
  gtrl_triangle.cxx  gtrl_triangle.h
  gtrl_vertex.h
  gtrl_polygon.cxx   gtrl_polygon.h
)

aux_source_directory(Templates gtrl_sources)

include( ${VXL_CMAKE_DIR}/FindNetlib.cmake )

if(NETLIB_FOUND)
  include_directories(${NETLIB_INCLUDE_DIR})

  set(gtrl_sources ${gtrl_sources}
    gtrl_triangulation.cxx gtrl_triangulation.h
  )
endif()

vxl_add_library(LIBRARY_NAME gtrl LIBRARY_SOURCES ${gtrl_sources})
target_link_libraries(gtrl ${VXL_LIB_PREFIX}vbl)
if(NETLIB_FOUND)
  target_link_libraries(gtrl ${NETLIB_LIBRARIES})
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
