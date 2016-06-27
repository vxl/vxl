if (HAS_GEOTIFF) # indirectly needed because of bvxm
# This is contrib/brl/bseg/brec/CMakeLists.txt

include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${MUL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bsta )
include_directories( ${BRL_INCLUDE_DIR}/bseg/ )
include( ${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    include_directories(${GEOTIFF_INCLUDE_DIR})
  endif()
endif()

set(brec_sources
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

aux_source_directory(Templates brec_sources)

vxl_add_library(LIBRARY_NAME brec LIBRARY_SOURCES ${brec_sources})

target_link_libraries(brec bvxm bbgm bsta_algo bgrl2 bsta bvgl bxml brip ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl)

add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/brec: requires geotiff")
endif()
