# ./vxl/vul/io/CMakeLists.txt

SET(vul_io_sources
  vul_io_user_info.h   vul_io_user_info.cxx
)

ADD_LIBRARY(vul_io ${vul_io_sources})
TARGET_LINK_LIBRARIES(vul_io vul vsl)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
