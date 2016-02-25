# bsta's Process Library

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories(${VXLCORE_INCLUDE_DIR})
include_directories( ${VTHREEP_INCLUDE_DIR}/rply )

set(bvrml_pro_sources
    bvrml_processes.h
    bvrml_register.h   bvrml_register.cxx
)

aux_source_directory(processes bvrml_pro_sources)
aux_source_directory(Templates bvrml_pro_sources)

vxl_add_library( LIBNAME bvrml_pro LIBSRCS ${bvrml_pro_sources} )

target_link_libraries(bvrml_pro boxm2_cpp bvrml ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_io bprb rply )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

