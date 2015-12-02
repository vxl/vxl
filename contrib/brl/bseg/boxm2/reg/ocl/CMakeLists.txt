
#depends on OPENCL being found...
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(boxm2_ocl_reg_sources
    boxm2_ocl_reg_mutual_info.h                  boxm2_ocl_reg_mutual_info.cxx
    boxm2_ocl_hierarchical_reg.h                 boxm2_ocl_hierarchical_reg.cxx
    boxm2_ocl_reg_points_to_volume_mutual_info.h  boxm2_ocl_reg_points_to_volume_mutual_info.cxx
    boxm2_ocl_hierarchical_points_to_volume_reg.h boxm2_ocl_hierarchical_points_to_volume_reg.cxx
   )

ADD_LIBRARY(boxm2_ocl_reg  ${boxm2_ocl_reg_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_reg boxm2_io  boxm2_ocl bocl brip baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2_ocl_reg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/reg/ocl/ ${boxm2_ocl_reg_sources})

add_subdirectory(exe)

ENDIF(OPENCL_FOUND)
