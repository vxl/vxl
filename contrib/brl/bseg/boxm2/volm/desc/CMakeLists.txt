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

add_library(boxm2_volm_desc ${boxm2_volm_desc_sources})

target_link_libraries(boxm2_volm_desc boxm2_volm boxm2 boxm2_io brip baio vpgl volm volm_desc vnl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
