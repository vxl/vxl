# mul/mil3d/CMakeLists.txt

SOURCE_FILES(mil3d_sources
 mil3d_image_3d.h
 mil3d_transform_3d.cxx mil3d_transform_3d.h
 mil3d_image_3d_of.txx  mil3d_image_3d_of.h  mil3d_image_3d_of.cxx
)

AUX_SOURCE_DIRECTORY(Templates mil3d_sources)

ADD_LIBRARY(mil3d mil3d_sources)
TARGET_LINK_LIBRARIES(mil3d mil)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
