#
# ./mul/vil/io/CMakeLists.txt
#
#   Ian Scott
#


ADD_LIBRARY(vil_io vil_io_sources )


SOURCE_FILES(vil_io_sources
vil_io_image
vil_io_image_impl
vil_io_memory_image
vil_io_memory_image_format
vil_io_memory_image_impl
)

AUX_SOURCE_DIRECTORY(Templates vil_io_sources )

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
