# This is contrib/brl/bbas/bwm/pro/CMakeLists.txt
# bwm's Process Library

# bwm depends on expat and bwm_pro depends on bwm primarily
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bwm_pro_sources
    bwm_processes.h
    bwm_register.h    bwm_register.cxx
)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )
  set(bwm_pro_sources ${bwm_pro_sources}
      processes/bwm_create_corr_file_process.cxx
     )
 endif()
endif()

aux_source_directory(Templates bwm_pro_sources)

vxl_add_library(LIBRARY_NAME bwm_pro LIBRARY_SOURCES ${bwm_pro_sources})
target_link_libraries( bwm_pro bprb brip bwm_io ${VXL_LIB_PREFIX}vul bwm_algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
