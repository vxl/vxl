# ./vxl/vul/io/CMakeLists.txt

set(vul_io_sources
  vul_io_user_info.h   vul_io_user_info.cxx
)

add_library(vul_io ${vul_io_sources})
target_link_libraries(vul_io vul vsl)

install_targets(/lib vul_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vul/io ${vul_io_sources})

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
