#
# ./vxl/vsl/CMakeLists.txt
#
#   Ian Scott
#

ADD_LIBRARY(vsl vsl_sources)

SOURCE_FILES(vsl_sources
  vsl_binary_io
  vsl_binary_loader_base
  vsl_indent
  vsl_test
)

AUX_SOURCE_DIRECTORY(Templates vsl_sources)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
