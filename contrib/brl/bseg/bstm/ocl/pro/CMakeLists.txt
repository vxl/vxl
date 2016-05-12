#depends on OPENCL being found...
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR} )


set(bstm_ocl_pro_sources
    bstm_ocl_processes.h
    bstm_ocl_register.h                     bstm_ocl_register.cxx
   )

aux_source_directory(processes bstm_ocl_pro_sources)

vxl_add_library(LIBRARY_NAME bstm_ocl_pro LIBRARY_SOURCES  ${bstm_ocl_pro_sources})
target_link_libraries(bstm_ocl_pro bstm_ocl bstm_ocl_algo boct brdb bprb ${VXL_LIB_PREFIX}vbl_io expatpp ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl)



if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
