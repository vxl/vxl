# brl/bpro/core/sdet_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories(${GEL_INCLUDE_DIR})
include_directories(${VTHREEP_INCLUDE_DIR})

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

vxl_add_library( LIBNAME sdet_pro LIBSRCS ${sdet_pro_sources})
target_link_libraries(sdet_pro bprb brdb sdet vil_pro ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
