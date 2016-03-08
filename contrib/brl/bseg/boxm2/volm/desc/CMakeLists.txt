# boxm2/volm/desc/CMakeLists.txt
# Index existence descriptor library

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/volm )

set(boxm2_volm_desc_sources
  boxm2_volm_desc_ex_indexer.h               boxm2_volm_desc_ex_indexer.cxx
  boxm2_volm_desc_ex_matcher.h               boxm2_volm_desc_ex_matcher.cxx
  boxm2_volm_desc_ex_land_only_indexer.h     boxm2_volm_desc_ex_land_only_indexer.cxx
  boxm2_volm_desc_ex_land_only_matcher.h     boxm2_volm_desc_ex_land_only_matcher.cxx
)

vxl_add_library(LIBRARY_NAME boxm2_volm_desc LIBRARY_SOURCES ${boxm2_volm_desc_sources})

target_link_libraries(boxm2_volm_desc boxm2_volm boxm2 boxm2_io brip baio ${VXL_LIB_PREFIX}vpgl volm volm_desc ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
