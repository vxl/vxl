# brl/bseg/boxm/sample/CMakeLists.txt
include_directories( ${MUL_INCLUDE_DIR} )
set(boxm_sample_sources
    boxm_edge_sample.h                        boxm_edge_sample.hxx
    boxm_edge_tangent_sample.h                boxm_edge_tangent_sample.hxx
    boxm_inf_line_sample.h                    boxm_inf_line_sample.hxx
    boxm_opt2_sample.h                        boxm_opt2_sample.hxx
    boxm_opt3_sample.h                        boxm_opt3_sample.hxx
    boxm_plane_obs.h                          boxm_plane_obs.hxx
    boxm_rt_sample.h                          boxm_rt_sample.hxx
    boxm_sample.h                             boxm_sample.hxx
    boxm_sample_multi_bin.h                   boxm_sample_multi_bin.hxx
    boxm_scalar_sample.h                      boxm_scalar_sample.hxx
    boxm_vis_implicit_sample.h
    boxm_update_sample.h
    dummy.cxx
   )

aux_source_directory(Templates boxm_sample_sources)

vxl_add_library(LIBRARY_NAME boxm_sample LIBRARY_SOURCES ${boxm_sample_sources})
target_link_libraries(boxm_sample boxm ${VXL_LIB_PREFIX}vgl_io bsta ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#processors for the samples
add_subdirectory(algo)
