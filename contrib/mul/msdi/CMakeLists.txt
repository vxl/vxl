# mul/msdi/CMakeLists.txt

doxygen_add_library(contrib/mul/msdi
  DEPENDS contrib/mul/msm contrib/mul/vimt 
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Shape Data Iterator Library"
)

SET(msdi_sources
  msdi_marked_images.h
  msdi_marked_images_from_files.cxx  msdi_marked_images_from_files.h
  msdi_array_of_marked_images.cxx    msdi_array_of_marked_images.h
  msdi_reflected_marked_images.cxx     msdi_reflected_marked_images.h
)

#AUX_SOURCE_DIRECTORY(Templates msdi_sources)

ADD_LIBRARY(msdi ${msdi_sources})
TARGET_LINK_LIBRARIES(msdi msm)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
