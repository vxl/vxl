# brl/bpro/core/icam_pro/CMakeLists.txt
# Brown vpgl processes

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set( icam_pro_sources
  icam_processes.h
  icam_register.h      icam_register.cxx
)
aux_source_directory(Templates icam_pro_sources)
aux_source_directory(processes icam_pro_sources)

add_library(icam_pro ${icam_pro_sources})
target_link_libraries(icam_pro bprb icam vpgl_algo vpgl_io vpgl vnl vil_io vil vbl vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
