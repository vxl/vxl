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

add_library(vil_pro ${vil_pro_sources})

target_link_libraries(vil_pro bbas_pro bprb bil bil_algo brip sdet vil_algo vil_io vil vnl vul vsl bsta_vis)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

install_targets(/lib vil_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bpro/core/vil_pro ${vil_pro_sources})
