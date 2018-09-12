# ./vxl/vul/io/CMakeLists.txt

set(vul_io_sources
  vul_io_user_info.h   vul_io_user_info.cxx
)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vul_io LIBRARY_SOURCES ${vul_io_sources})
target_link_libraries(${VXL_LIB_PREFIX}vul_io ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl)

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vul_io-obj OBJECT ${vul_io_sources})
endif()
