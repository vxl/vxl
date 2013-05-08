#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )


SET(bstm_ocl_pro_sources
    bstm_ocl_processes.h
    bstm_ocl_register.h                     bstm_ocl_register.cxx
   )

AUX_SOURCE_DIRECTORY(processes bstm_ocl_pro_sources)

ADD_LIBRARY(bstm_ocl_pro  ${bstm_ocl_pro_sources})
TARGET_LINK_LIBRARIES(bstm_ocl_pro bstm_ocl bstm_ocl_algo  boct brdb vbl_io expatpp vpgl vgl vil vul vcl)

INSTALL_TARGETS(/lib bstm_ocl_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/ocl/pro ${bstm_ocl_pro_sources})


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
