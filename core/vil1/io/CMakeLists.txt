# vxl/vil1/io/CMakeLists.txt

SET(vil1_io_sources
  vil1_io_image.cxx vil1_io_image.h
  vil1_io_image_impl.cxx vil1_io_image_impl.h
  vil1_io_memory_image.cxx vil1_io_memory_image.h
  vil1_io_memory_image_format.cxx vil1_io_memory_image_format.h
  vil1_io_memory_image_impl.cxx vil1_io_memory_image_impl.h

  vil1_io_memory_image_of.txx vil1_io_memory_image_of.h
  vil1_io_rgb.txx vil1_io_rgb.h
  vil1_io_rgba.txx vil1_io_rgba.h
)

AUX_SOURCE_DIRECTORY(Templates vil1_io_sources)

ADD_LIBRARY(vil1_io ${vil1_io_sources})
TARGET_LINK_LIBRARIES( vil1_io vil1 vsl )
INSTALL_TARGETS(/lib vil1_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vil1/io ${vil1_io_sources})

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
