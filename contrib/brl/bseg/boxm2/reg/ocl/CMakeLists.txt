
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

vxl_add_library(LIBRARY_NAME boxm2_ocl_reg LIBRARY_SOURCES  ${boxm2_ocl_reg_sources})
target_link_libraries(boxm2_ocl_reg boxm2_io  boxm2_ocl bocl brip baio ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl depth_map)

#install the .h .hxx and libs

add_subdirectory(exe)

endif()
