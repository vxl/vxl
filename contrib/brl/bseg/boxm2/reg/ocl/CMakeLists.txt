
#depends on OPENCL being found...
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )


SET(boxm2_ocl_reg_sources
    boxm2_ocl_reg_mutual_info.h   boxm2_ocl_reg_mutual_info.cxx
	boxm2_ocl_reg_minfo_func.h	  boxm2_ocl_reg_minfo_func.cxx
   )

ADD_LIBRARY(boxm2_ocl_reg  ${boxm2_ocl_reg_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_reg boxm2_io boxm2_reg boxm2_ocl bocl brip baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2_ocl_reg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/reg/ocl/ ${boxm2_ocl_reg_sources})

SUBDIRS(exe)
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(OPENCL_FOUND)
