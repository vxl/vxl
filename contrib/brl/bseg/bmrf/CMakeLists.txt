# Brown Markov Random Field library
# brl/bseg/bmrf/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bmrf_sources
    bmrf_node.cxx              bmrf_node.h               bmrf_node_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates bmrf_sources)

ADD_LIBRARY(bmrf ${bmrf_sources})

TARGET_LINK_LIBRARIES(bmrf vbl vsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)



