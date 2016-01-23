# bsta's Process Library

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bsta_pro_sources
    bsta_processes.h
    bsta_register.h   bsta_register.cxx
)

aux_source_directory(Templates bsta_pro_sources)
aux_source_directory(processes bsta_pro_sources)

add_library(bsta_pro ${bsta_pro_sources} )

target_link_libraries(bsta_pro bsta_algo bsta_io bsta bprb brdb brip vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

install_targets(/lib bsta_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsta/pro ${bsta_pro_sources})
