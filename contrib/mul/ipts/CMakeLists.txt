# mul/ipts/CMakeLists.txt

SET(ipts_sources
    ipts_scale_space_peaks.h
    ipts_draw.h
    ipts_local_entropy.cxx ipts_local_entropy.h
    ipts_corner_pyramid.cxx ipts_corner_pyramid.h
    ipts_entropy_pyramid.cxx ipts_entropy_pyramid.h
)
AUX_SOURCE_DIRECTORY(Templates ipts_sources)

ADD_LIBRARY(ipts ${ipts_sources})
TARGET_LINK_LIBRARIES(ipts vimt mbl)

SUBDIRS(tools)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
