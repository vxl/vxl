# gel/gtrl/CMakeLists.txt

doxygen_add_library(contrib/gel/gtrl
  DEPENDS core/vbl v3p/netlib
  PACKAGE contrib/gel
  DESCRIPTION "Polygon triangulation"
)

SET(gtrl_sources
  gtrl_triangle.cxx  gtrl_triangle.h
  gtrl_vertex.h
  gtrl_polygon.cxx   gtrl_polygon.h
)

AUX_SOURCE_DIRECTORY(Templates gtrl_sources)

INCLUDE( ${VXL_CMAKE_DIR}/FindNetlib.cmake )

IF(NETLIB_FOUND)
  INCLUDE_DIRECTORIES(${NETLIB_INCLUDE_DIR})

  SET(gtrl_sources ${gtrl_sources}
    gtrl_triangulation.cxx gtrl_triangulation.h
  )
ENDIF(NETLIB_FOUND)

ADD_LIBRARY(gtrl ${gtrl_sources})
INSTALL_TARGETS(/lib gtrl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/gtrl ${gtrl_sources})

TARGET_LINK_LIBRARIES(gtrl vbl)
IF(NETLIB_FOUND)
  TARGET_LINK_LIBRARIES(gtrl ${NETLIB_LIBRARIES})
ENDIF(NETLIB_FOUND)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
