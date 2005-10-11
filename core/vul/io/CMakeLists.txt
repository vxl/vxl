# ./vxl/vul/io/CMakeLists.txt

SET(vul_io_sources
  vul_io_user_info.h   vul_io_user_info.cxx
)

ADD_LIBRARY(vul_io ${vul_io_sources})
TARGET_LINK_LIBRARIES(vul_io vul vsl)

INSTALL_TARGETS(/lib vul_io)
INSTALL_NOBASE_HEADER_FILES(/include/core/vul/io ${vul_io_sources})

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
