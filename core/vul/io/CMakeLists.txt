# ./vxl/vul/io/CMakeLists.txt

ADD_LIBRARY(vul_io vul_io_sources)

SOURCE_FILES(vul_io_sources
  vul_io_user_info.h   vul_io_user_info.cxx
)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
