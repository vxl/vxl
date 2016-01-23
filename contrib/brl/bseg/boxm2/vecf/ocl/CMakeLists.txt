
#depends on OPENCL being found...
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)

add_definitions(-DHAS_OPENCL=1)
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(boxm2_vecf_ocl_sources
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
    boxm2_vecf_ocl_composite_transform.h        boxm2_vecf_ocl_composite_transform.cxx
    boxm2_vecf_ocl_composite_head_model.h       boxm2_vecf_ocl_composite_head_model.cxx
    boxm2_vecf_ocl_head_model.h                 boxm2_vecf_ocl_head_model.cxx
    boxm2_vecf_ocl_appearance_extractor.h       boxm2_vecf_ocl_appearance_extractor.cxx
    boxm2_vecf_ocl_orbit_vector_field.h         boxm2_vecf_ocl_orbit_vector_field.cxx

   )

aux_source_directory(Templates boxm2_vecf_ocl_sources)

add_library(boxm2_vecf_ocl  ${boxm2_vecf_ocl_sources})
target_link_libraries(boxm2_vecf_ocl boxm2_vecf boxm2_io  boxm2_ocl boxm2_ocl_algo bocl brip baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)

#install the .h .txx and libs
install_targets(/lib boxm2_vecf_ocl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/vecf/ocl/ ${boxm2_vecf_ocl_sources})

#processes
subdirs(pro)


#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

endif(OPENCL_FOUND)
