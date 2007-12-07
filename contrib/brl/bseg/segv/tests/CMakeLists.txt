# brl/bbas/bseg/segv/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})

ADD_EXECUTABLE(segv_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(segv_test_include segv)
