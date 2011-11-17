# This is contrib/brl/bseg/brec/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bsta )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/ )
INCLUDE( ${VXL_CMAKE_DIR}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
  ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)

SET(brec_sources
    brec_bayesian_propagation.h  brec_bayesian_propagation.cxx
    brec_param_estimation.h  brec_param_estimation.cxx
    brec_fg_bg_pair_density.h  brec_fg_bg_pair_density.cxx
    brec_fg_pair_density.h  brec_fg_pair_density.cxx
    brec_pair_density.h          brec_pair_density.cxx
    brec_bg_pair_density.h  brec_bg_pair_density.cxx
    brec_part_base.h     brec_part_base.cxx
    brec_part_base_sptr.h
    brec_part_gaussian.h    brec_part_gaussian.cxx
    brec_part_gaussian_sptr.h
    brec_part_hierarchy.h    brec_part_hierarchy.cxx
    brec_part_hierarchy_sptr.h
    brec_hierarchy_edge.h    brec_hierarchy_edge.cxx
    brec_hierarchy_edge_sptr.h
    brec_part_hierarchy_builder.h    brec_part_hierarchy_builder.cxx
    brec_part_hierarchy_detector.h    brec_part_hierarchy_detector.cxx
    brec_part_hierarchy_detector_sptr.h
    brec_glitch.h    brec_glitch.cxx
    brec_part_hierarchy_learner.h    brec_part_hierarchy_learner.cxx
    brec_part_hierarchy_learner_sptr.h
   )

AUX_SOURCE_DIRECTORY(Templates brec_sources)

ADD_LIBRARY(brec ${brec_sources})

TARGET_LINK_LIBRARIES(brec bvxm bbgm bsta_algo bgrl2 bsta bvgl bxml brip vil_algo vgl_algo vnl_algo vnl vgl vil vul vbl_io vbl)

SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
