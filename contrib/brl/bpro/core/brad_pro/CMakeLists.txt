# brad's Process Library

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(brad_pro_sources
    brad_processes.h
    brad_register.h   brad_register.cxx
)
aux_source_directory(Templates brad_pro_sources)
aux_source_directory(processes brad_pro_sources)

add_library(brad_pro ${brad_pro_sources} )

target_link_libraries(brad_pro brad_io brad bprb brdb brip ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl vil_pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

install_targets(/lib brad_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bpro/core/brad_pro ${brad_pro_sources})
