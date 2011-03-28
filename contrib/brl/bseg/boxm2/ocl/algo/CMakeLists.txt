#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
SET(boxm2_ocl_algo_sources
    boxm2_ocl_render_expected_image_function.h boxm2_ocl_render_expected_image_function.cxx  
  )

ADD_LIBRARY(boxm2_ocl_algo  ${boxm2_ocl_algo_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_algo boxm2_ocl)

