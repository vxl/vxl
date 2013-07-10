#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

# TIFF and GEOTIFF needed for extracting coordinate information from buckeye lidar images
INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
ENDIF(TIFF_FOUND)

INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
IF(GEOTIFF_FOUND)
  INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )
ENDIF(GEOTIFF_FOUND)


SET(boxm2_ocl_pro_sources
    boxm2_ocl_processes.h
    boxm2_ocl_register.h                     boxm2_ocl_register.cxx
    processes/boxm2_ocl_filter_process.h
    processes/boxm2_ocl_kernel_filter_process.h
    processes/boxm2_ocl_kernel_vector_filter_process.h
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_ocl_pro_sources)
AUX_SOURCE_DIRECTORY(processes boxm2_ocl_pro_sources)

ADD_LIBRARY(boxm2_ocl_pro  ${boxm2_ocl_pro_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_pro boxm2_ocl_algo boct brdb vbl_io expatpp vpgl vgl vgl_xio vil vul vbl vsl vcl bvpl_kernels brad bsta_algo)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib boxm2_ocl_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ocl/pro ${boxm2_ocl_pro_sources})
