#depends on OPENCL being found...
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR} )

# TIFF and GEOTIFF needed for extracting coordinate information from buckeye lidar images
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
endif(TIFF_FOUND)

include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
if(GEOTIFF_FOUND)
  include_directories( ${GEOTIFF_INCLUDE_DIR} )
endif(GEOTIFF_FOUND)


set(boxm2_ocl_pro_sources
    boxm2_ocl_processes.h
    boxm2_ocl_register.h                     boxm2_ocl_register.cxx
    processes/boxm2_ocl_filter_process.h
    processes/boxm2_ocl_kernel_filter_process.h
    processes/boxm2_ocl_kernel_vector_filter_process.h
   )

aux_source_directory(Templates boxm2_ocl_pro_sources)
aux_source_directory(processes boxm2_ocl_pro_sources)

add_library(boxm2_ocl_pro  ${boxm2_ocl_pro_sources} )
target_link_libraries(boxm2_ocl_pro boxm2_ocl_algo boct brdb vbl_io expatpp vpgl vgl vgl_xio vil vul vbl vsl vcl bvpl_kernels brad bsta_algo vpgl_pro)
# Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

install_targets(/lib boxm2_ocl_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ocl/pro ${boxm2_ocl_pro_sources})
