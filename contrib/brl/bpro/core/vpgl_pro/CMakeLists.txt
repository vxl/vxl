# brl/bpro/core/vpgl_pro/CMakeLists.txt
# Brown vpgl processes

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${GEL_INCLUDE_DIR})
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )
 endif()
endif()

set( vpgl_pro_sources
  vpgl_processes.h
  vpgl_register.h      vpgl_register.cxx
)
aux_source_directory(Templates vpgl_pro_sources)
aux_source_directory(processes vpgl_pro_sources)

vxl_add_library( LIBNAME vpgl_pro LIBSRCS ${vpgl_pro_sources})
target_link_libraries(vpgl_pro bprb vsph bpgl bpgl_algo ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul bkml brad bbas_pro vsol)


if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
