#
# ./mul/vbl/io/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(vbl_io vbl_io_sources)



AUX_SOURCE_DIRECTORY(Templates vbl_io_sources)


IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
