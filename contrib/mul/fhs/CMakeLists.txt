# mul/fhs/CMakeLists.txt

doxygen_add_library(contrib/mul/fhs
  DEPENDS contrib/mul/vimt contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Feature matching using Felzenszwalb and Huttenlocher's method"
  )

SET(fhs_sources
  fhs_arc.cxx       fhs_arc.h
  fhs_searcher.cxx  fhs_searcher.h
)
AUX_SOURCE_DIRECTORY(Templates fhs_sources)

ADD_LIBRARY(fhs ${fhs_sources})
TARGET_LINK_LIBRARIES(fhs vimt vnl vil_algo vgl vsl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

IF(BUILD_MUL_TOOLS)
  SUBDIRS(tools)
ENDIF(BUILD_MUL_TOOLS)
