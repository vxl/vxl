# mul/vil2/io/CMakeLists.txt

SET(vil2_io_sources
  vil2_io_memory_chunk.cxx vil2_io_memory_chunk.h
  vil2_io_smart_ptr.txx    vil2_io_smart_ptr.h
  vil2_io_image_view.txx   vil2_io_image_view.h
)

AUX_SOURCE_DIRECTORY(Templates vil2_io_sources)

ADD_LIBRARY(vil2_io ${vil2_io_sources})
TARGET_LINK_LIBRARIES( vil2_io vil2 vsl )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
