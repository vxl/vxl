# core/vil/io/CMakeLists.txt

SET(vil_io_sources
  vil_io_memory_chunk.cxx vil_io_memory_chunk.h
  vil_io_smart_ptr.txx    vil_io_smart_ptr.h
  vil_io_image_view.h
)

AUX_SOURCE_DIRECTORY(Templates vil_io_sources)

ADD_LIBRARY(vil_io ${vil_io_sources})
TARGET_LINK_LIBRARIES( vil_io vil vsl )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
