# brl/bseg/boxm2/sample/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm2_sample_sources
    boxm_edge_sample.h                        boxm_edge_sample.txx
    boxm_edge_tangent_sample.h                boxm_edge_tangent_sample.txx
    boxm_inf_line_sample.h                    boxm_inf_line_sample.txx
    boxm_opt2_sample.h                        boxm_opt2_sample.txx
    boxm_opt3_sample.h                        boxm_opt3_sample.txx
    boxm_plane_obs.h                          boxm_plane_obs.txx
    boxm_rt_sample.h                          boxm_rt_sample.txx
    boxm_sample.h                             boxm_sample.txx
    boxm_sample_multi_bin.h                   boxm_sample_multi_bin.txx
    boxm_scalar_sample.h                      boxm_scalar_sample.txx
    boxm_vis_implicit_sample.h
    dummy.cxx
   )

ADD_LIBRARY(boxm2_sample ${boxm2_sample_sources})

AUX_SOURCE_DIRECTORY(Templates boxm2_sample_sources)

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm2_sample boxm2 vil vsl vcl vgl vgl_io bsta)

#processors for the samples
SUBDIRS(algo)
