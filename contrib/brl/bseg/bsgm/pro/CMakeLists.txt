# Process for SGM
# brl/bseg/bsgm/pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bpro )

set( bsgm_pro_sources
     bsgm_register.h  bsgm_register.cxx
     bsgm_processes.h
)

aux_source_directory(processes bsgm_pro_sources)

vxl_add_library( LIBRARY_NAME bsgm_pro LIBRARY_SOURCES ${bsgm_pro_sources} )
target_link_libraries( bsgm_pro bsgm bprb brdb ${VXL_LIB_PREFIX}vil )
