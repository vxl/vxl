# brl/bseg/boxm/sample/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_sample_sources
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
    boxm_update_sample.h
    dummy.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm_sample_sources)

ADD_LIBRARY(boxm_sample ${boxm_sample_sources})
TARGET_LINK_LIBRARIES(boxm_sample boxm vgl_io bsta vil vgl vbl vsl vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#processors for the samples
SUBDIRS(algo)
