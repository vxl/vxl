# brl/bbas/bseg/vpro/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

ADD_EXECUTABLE(vpro_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(vpro_test_include vpro)
