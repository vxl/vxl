#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
SET(bstm_ocl_algo_sources
    bstm_ocl_render_expected_image_function.h  bstm_ocl_render_expected_image_function.cxx
   )

ADD_LIBRARY(bstm_ocl_algo  ${bstm_ocl_algo_sources})
TARGET_LINK_LIBRARIES(bstm_ocl_algo bstm_ocl boxm2_ocl_algo vgl vil vul)

INSTALL_TARGETS(/lib bstm_ocl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/ocl/algo ${bstm_ocl_algo_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
