#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
SET(boxm2_ocl_pro_sources
    boxm2_ocl_processes.h
    boxm2_ocl_register.h                     boxm2_ocl_register.cxx
  )

AUX_SOURCE_DIRECTORY(Templates boxm2_ocl_pro_sources)
AUX_SOURCE_DIRECTORY(processes boxm2_ocl_pro_sources)

ADD_LIBRARY(boxm2_ocl_pro  ${boxm2_ocl_pro_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_pro boxm2_ocl_algo boct brdb vbl_io expatpp vpgl vgl vgl_xio vbl vsl vcl)

#tests
SUBDIRS(tests)
