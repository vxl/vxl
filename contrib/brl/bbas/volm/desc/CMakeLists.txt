# volm/desc/CMakeLists.txt
# Existence matching and base matching library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/volm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bsta )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )

SET(volm_desc_sources
  volm_descriptor_factory.h               volm_descriptor_factory.cxx
  volm_existance_descriptor_factory.h     volm_existance_descriptor_factory.cxx
  volm_upcount_descriptor_factory.h       volm_upcount_descriptor_factory.cxx
)

AUX_SOURCE_DIRECTORY(Templates volm_desc_sources)

ADD_LIBRARY(volm_desc ${volm_desc_sources})

TARGET_LINK_LIBRARIES(volm_desc volm bsta bsta_algo bsta_vis bsvg bvgl vsl vbl vbl_io vul depth_map)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)