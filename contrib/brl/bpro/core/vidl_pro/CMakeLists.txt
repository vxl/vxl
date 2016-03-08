# brl/bpro/core/vidl_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )

set(HAS_DIRECTSHOW 0)
if(DIRECTSHOW_FOUND)
  set(HAS_DIRECTSHOW 1)
  include_directories(${DIRECTSHOW_INCLUDE_DIRS})
endif()

set(vidl_pro_sources
    vidl_io_istream.h             vidl_io_istream.cxx
    vidl_io_ostream.h             vidl_io_ostream.cxx
    vidl_pro_utils.h              vidl_pro_utils.cxx
    vidl_register.h               vidl_register.cxx
    vidl_processes.h
   )
aux_source_directory(Templates vidl_pro_sources)
aux_source_directory(processes vidl_pro_sources)

vxl_add_library(LIBRARY_NAME vidl_pro LIBRARY_SOURCES ${vidl_pro_sources})
target_link_libraries(vidl_pro bprb brdb ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
