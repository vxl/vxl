# brl/bpro/core/sdet_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories(${GEL_INCLUDE_DIR})
include_directories(${VTHREEP_INCLUDE_DIR})
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )
 endif()
endif()

set(sdet_pro_sources
    sdet_processes.h
    sdet_register.h             sdet_register.cxx
    processes/sdet_detect_edges_process.h
    processes/sdet_detect_edge_tangent_process.h
    processes/sdet_detect_edge_tangent_interp_process.h
    processes/sdet_detect_third_order_edges_process.h
    processes/sdet_segment_image_process.h
   )

aux_source_directory(Templates sdet_pro_sources)
aux_source_directory(processes sdet_pro_sources)

vxl_add_library(LIBRARY_NAME sdet_pro LIBRARY_SOURCES ${sdet_pro_sources})
target_link_libraries(sdet_pro bprb brdb sdet volm vil_pro ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpgl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
