
#depends on OPENCL being found...
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(boxm2_vecf_ocl_sources
    boxm2_vecf_ocl_transform_scene.h                  boxm2_vecf_ocl_transform_scene.cxx
    boxm2_vecf_ocl_ssd_func.h                         boxm2_vecf_ocl_ssd_func.cxx
    boxm2_vecf_ocl_transform_minimizer.h              boxm2_vecf_ocl_transform_minimizer.cxx
    boxm2_vecf_ocl_store_nbrs.h                       boxm2_vecf_ocl_store_nbrs.cxx
    boxm2_vecf_ocl_filter.h                           boxm2_vecf_ocl_filter.cxx
    boxm2_vecf_ocl_transform_scene_sptr.h
    boxm2_vecf_ocl_orbit_scene.h                boxm2_vecf_ocl_orbit_scene.cxx
    boxm2_vecf_ocl_vector_field.h
    boxm2_vecf_ocl_vector_field_adaptor.h       boxm2_vecf_ocl_vector_field_adaptor.cxx
    boxm2_vecf_ocl_similarity_transform.h       boxm2_vecf_ocl_similarity_transform.cxx
    boxm2_vecf_ocl_landmark_warp.h              boxm2_vecf_ocl_landmark_warp.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_vecf_ocl_sources)

ADD_LIBRARY(boxm2_vecf_ocl  ${boxm2_vecf_ocl_sources})
TARGET_LINK_LIBRARIES(boxm2_vecf_ocl boxm2_vecf boxm2_io  boxm2_ocl boxm2_ocl_algo bocl brip baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2_vecf_ocl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/vecf/ocl/ ${boxm2_vecf_ocl_sources})

#processes
subdirs(pro)


#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(OPENCL_FOUND)
