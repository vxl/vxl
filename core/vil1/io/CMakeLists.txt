# vxl/vil/io/CMakeLists.txt

SOURCE_FILES(vil_io_sources
  vil_io_image.cxx vil_io_image.h
  vil_io_image_impl.cxx vil_io_image_impl.h
  vil_io_memory_image.cxx vil_io_memory_image.h
  vil_io_memory_image_format.cxx vil_io_memory_image_format.h
  vil_io_memory_image_impl.cxx vil_io_memory_image_impl.h

  vil_io_memory_image_of.txx vil_io_memory_image_of.h
  vil_io_rgb.txx vil_io_rgb.h
  vil_io_rgba.txx vil_io_rgba.h
)

AUX_SOURCE_DIRECTORY(Templates vil_io_sources)

ADD_LIBRARY(vil_io vil_io_sources)
TARGET_LINK_LIBRARIES( vil_io vil vsl )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
