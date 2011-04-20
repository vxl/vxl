# Brown radiometry library
# brl/bbas/brad/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(brad_sources
  dummy.cxx
)

ADD_LIBRARY(brad ${brad_sources})
INSTALL_TARGETS(/lib brad)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/brad ${brad_sources})
TARGET_LINK_LIBRARIES(brad vgl)
