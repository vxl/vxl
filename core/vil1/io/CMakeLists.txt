# ./vxl/vil/io/CMakeLists.txt

ADD_LIBRARY(vil_io vil_io_sources )

SOURCE_FILES(vil_io_sources
  vil_io_image.cxx
  vil_io_image_impl.cxx
  vil_io_memory_image.cxx
  vil_io_memory_image_format.cxx
  vil_io_memory_image_impl.cxx
)

AUX_SOURCE_DIRECTORY(Templates vil_io_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vil/io/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
