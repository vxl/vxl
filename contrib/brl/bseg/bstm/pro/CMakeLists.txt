include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${MUL_INCLUDE_DIR} )
set(bstm_pro_sources
    bstm_register.h                              bstm_register.cxx
    bstm_processes.h
   )
aux_source_directory(processes bstm_pro_sources)
vxl_add_library(LIBRARY_NAME bstm_pro LIBRARY_SOURCES  ${bstm_pro_sources})

target_link_libraries(bstm_pro bprb boxm2_util bstm bstm_io bstm_util boxm2_basic boct brdb ${VXL_LIB_PREFIX}vpgl_pro ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
