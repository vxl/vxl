# mul/vil3d/io/CMakeLists.txt

SET(vil3d_io_sources
  vil3d_io_image_view.h
)

AUX_SOURCE_DIRECTORY(Templates vil3d_io_sources)

ADD_LIBRARY(vil3d_io ${vil3d_io_sources})
TARGET_LINK_LIBRARIES( vil3d_io vil3d vil2_io vil2 vsl )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
