# mul/vil3d/io/CMakeLists.txt

SET(vil3d_io_sources
    vil3d_io_image_view.h
    # force building of a library
    vil3d_io_dummy.cxx
)

ADD_LIBRARY(vil3d_io ${vil3d_io_sources})
TARGET_LINK_LIBRARIES( vil3d_io vil3d vil_io vil vsl )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
