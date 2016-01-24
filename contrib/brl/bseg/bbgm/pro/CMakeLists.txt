# brl/bseg/bbgm/pro/CMakeLists.txt

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${GEL_INCLUDE_DIR} )
set(bbgm_pro_sources
    bbgm_register.h                   bbgm_register.cxx
    bbgm_processes.h
)

aux_source_directory(Templates bbgm_pro_sources)
aux_source_directory(processes bbgm_pro_sources)

add_library(bbgm_pro ${bbgm_pro_sources})
target_link_libraries(bbgm_pro bbgm bprb brdb bsta_algo bsta brip vidl vil vil_pro vbl_io vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
