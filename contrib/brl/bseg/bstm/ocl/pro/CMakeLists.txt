#depends on OPENCL being found...
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR} )


set(bstm_ocl_pro_sources
    bstm_ocl_processes.h
    bstm_ocl_register.h                     bstm_ocl_register.cxx
   )

aux_source_directory(processes bstm_ocl_pro_sources)

add_library(bstm_ocl_pro  ${bstm_ocl_pro_sources})
target_link_libraries(bstm_ocl_pro bstm_ocl bstm_ocl_algo boct brdb bprb vbl_io expatpp vpgl vgl vil vul vcl)

install_targets(/lib bstm_ocl_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/ocl/pro ${bstm_ocl_pro_sources})


if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
