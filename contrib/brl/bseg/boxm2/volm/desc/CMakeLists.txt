# boxm2/volm/desc/CMakeLists.txt
# Index existance descriptor library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/volm )

SET(boxm2_volm_desc_sources
  boxm2_volm_desc_ex_indexer.h               boxm2_volm_desc_ex_indexer.cxx
  boxm2_volm_desc_ex_matcher.h               boxm2_volm_desc_ex_matcher.cxx
  boxm2_volm_desc_ex_land_only_indexer.h     boxm2_volm_desc_ex_land_only_indexer.cxx
  boxm2_volm_desc_ex_land_only_matcher.h     boxm2_volm_desc_ex_land_only_matcher.cxx
)

ADD_LIBRARY(boxm2_volm_desc ${boxm2_volm_desc_sources})

TARGET_LINK_LIBRARIES(boxm2_volm_desc boxm2_volm boxm2 boxm2_io brip baio vpgl volm volm_desc vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(exe)