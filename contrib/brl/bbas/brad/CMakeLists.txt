# basic radiometery library

  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

  SET(brad_sources
      brad_illum_util.h  brad_illum_util.cxx brad_illum_cost_function.h
  )
  AUX_SOURCE_DIRECTORY(Templates brad_sources)
  ADD_LIBRARY(brad ${brad_sources} )

  TARGET_LINK_LIBRARIES(brad bsta vnl vnl_algo vgl vil vbl vul )

#tests
SUBDIRS(tests)

