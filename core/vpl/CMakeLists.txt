#
# ./vxl/vpl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vpl vpl_sources)

SOURCE_FILES( vpl_sources
  vpl
)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
