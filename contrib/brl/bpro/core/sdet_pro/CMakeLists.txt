# brl/bpro/core/sdet_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(sdet_pro_sources
    sdet_processes.h
    sdet_register.h             sdet_register.cxx
    processes/sdet_detect_edges_process.h
    processes/sdet_detect_edge_tangent_process.h
    processes/sdet_detect_edge_tangent_interp_process.h
    processes/sdet_detect_third_order_edges_process.h
    processes/sdet_segment_image_process.h
   )

AUX_SOURCE_DIRECTORY(Templates sdet_pro_sources)
AUX_SOURCE_DIRECTORY(processes sdet_pro_sources)

ADD_LIBRARY(sdet_pro ${sdet_pro_sources})
TARGET_LINK_LIBRARIES(sdet_pro bprb brdb sdet vil_pro vil vgl vsl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
