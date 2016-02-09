# ./vxl/vul/io/CMakeLists.txt

set(vul_io_sources
  vul_io_user_info.h   vul_io_user_info.cxx
)

vxl_add_library(LIBNAME vul_io LIBSRCS ${vul_io_sources})
target_link_libraries(vul_io vul vsl)

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
