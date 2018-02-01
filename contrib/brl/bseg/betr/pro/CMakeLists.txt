include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
set(betr_pro_sources
    betr_register.h                              betr_register.cxx
    betr_processes.h
   )
aux_source_directory(processes betr_pro_sources)
vxl_add_library(LIBRARY_NAME betr_pro LIBRARY_SOURCES  ${betr_pro_sources})

target_link_libraries(betr_pro betr bprb brdb ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
