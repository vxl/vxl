# brl/bpro/core/vil_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR} )

set(vil_pro_sources
    vil_math_functors.h
    vil_register.h                       vil_register.cxx
    vil_processes.h
)

aux_source_directory(Templates vil_pro_sources)

aux_source_directory(processes vil_pro_sources)

vxl_add_library(LIBRARY_NAME vil_pro LIBRARY_SOURCES ${vil_pro_sources})

target_link_libraries(vil_pro bbas_pro bprb bil bil_algo brip sdet ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl bsta_vis)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()