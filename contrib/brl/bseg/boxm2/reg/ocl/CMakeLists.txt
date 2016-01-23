
#depends on OPENCL being found...
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)

add_definitions(-DHAS_OPENCL=1)
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(boxm2_ocl_reg_sources
    boxm2_ocl_reg_mutual_info.h                  boxm2_ocl_reg_mutual_info.cxx
    boxm2_ocl_hierarchical_reg.h                 boxm2_ocl_hierarchical_reg.cxx
    boxm2_ocl_reg_points_to_volume_mutual_info.h  boxm2_ocl_reg_points_to_volume_mutual_info.cxx
    boxm2_ocl_hierarchical_points_to_volume_reg.h boxm2_ocl_hierarchical_points_to_volume_reg.cxx
   )

add_library(boxm2_ocl_reg  ${boxm2_ocl_reg_sources})
target_link_libraries(boxm2_ocl_reg boxm2_io  boxm2_ocl bocl brip baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)

#install the .h .txx and libs
install_targets(/lib boxm2_ocl_reg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/reg/ocl/ ${boxm2_ocl_reg_sources})

add_subdirectory(exe)

endif(OPENCL_FOUND)
