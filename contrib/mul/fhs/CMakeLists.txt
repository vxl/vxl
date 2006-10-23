# mul/fhs/CMakeLists.txt

SET(fhs_sources
  fhs_arc.cxx       fhs_arc.h
  fhs_searcher.cxx  fhs_searcher.h
)
AUX_SOURCE_DIRECTORY(Templates fhs_sources)

ADD_LIBRARY(fhs ${fhs_sources})
TARGET_LINK_LIBRARIES(fhs mbl vimt vnl vil_algo vil vsl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

SUBDIRS(tools)
