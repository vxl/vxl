# core/vil/io/CMakeLists.txt

SET(vil_io_sources
  vil_io_memory_chunk.cxx vil_io_memory_chunk.h
  vil_io_smart_ptr.txx    vil_io_smart_ptr.h
  vil_io_image_view.h
)
AUX_SOURCE_DIRECTORY(Templates vil_io_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(Templates/vsl_vector_io+vcl_vector+vil_image_view+float---.cxx
                              PROPERTIES
                              COMPILE_FLAGS -ftemplate-depth-35)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(vil_io ${vil_io_sources})
TARGET_LINK_LIBRARIES( vil_io vil vsl )

INSTALL_TARGETS(/lib vil_io)
INSTALL_NOBASE_HEADER_FILES(/include/core/vil/io ${vil_io_sources})

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
