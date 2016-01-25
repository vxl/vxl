#depends on OPENCL being found...
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR} )

# TIFF and GEOTIFF needed for extracting coordinate information from buckeye lidar images
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
endif()

include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
if(GEOTIFF_FOUND)
  include_directories( ${GEOTIFF_INCLUDE_DIR} )
endif()


set(boxm2_vecf_ocl_pro_sources
    boxm2_vecf_ocl_processes.h
    boxm2_vecf_ocl_register.h                     boxm2_vecf_ocl_register.cxx
   )

aux_source_directory(Templates boxm2_vecf_ocl_pro_sources)
aux_source_directory(processes boxm2_vecf_ocl_pro_sources)

add_library(boxm2_vecf_ocl_pro  ${boxm2_vecf_ocl_pro_sources})
target_link_libraries(boxm2_vecf_ocl_pro bprb boxm2_vecf_ocl boct brdb vbl_io expatpp vpgl vgl vgl_xio vil vul vbl vsl vcl bvpl_kernels brad bsta_algo)

#tests
if( BUILD_TESTING )
#  add_subdirectory(tests)
endif()

install_targets(/lib boxm2_vecf_ocl_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/vecf/ocl/pro ${boxm2_vecf_ocl_pro_sources})
