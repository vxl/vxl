# Brown Markov Random Field library
# brl/bseg/bmrf/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bmrf_sources
    bmrf_epipole.cxx           bmrf_epipole.h
    bmrf_epi_point.cxx         bmrf_epi_point.h          bmrf_epi_point_sptr.h
    bmrf_epi_seg.cxx           bmrf_epi_seg.h            bmrf_epi_seg_sptr.h
    bmrf_gamma_func.cxx        bmrf_gamma_func.h         bmrf_gamma_func_sptr.h
    bmrf_epi_transform.cxx     bmrf_epi_transform.h
    bmrf_arc.cxx               bmrf_arc.h                bmrf_arc_sptr.h
    bmrf_node.cxx              bmrf_node.h               bmrf_node_sptr.h
    bmrf_network.cxx           bmrf_network.h            bmrf_network_sptr.h
    bmrf_network_builder_params.cxx   bmrf_network_builder_params.h  
    bmrf_network_builder.cxx   bmrf_network_builder.h   
    bmrf_curvel_3d.cxx         bmrf_curvel_3d.h          bmrf_curvel_3d_sptr.h
    bmrf_curve_3d_builder.cxx  bmrf_curve_3d_builder.h
)

AUX_SOURCE_DIRECTORY(Templates bmrf_sources)

ADD_LIBRARY(bmrf ${bmrf_sources})

TARGET_LINK_LIBRARIES(bmrf brip bugl vtol gevd vdgl vil vbl vnl vgl vsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
