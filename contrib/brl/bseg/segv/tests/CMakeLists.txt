# brl/bbas/bseg/segv/tests/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR})

add_executable(segv_test_include test_include.cxx)
target_link_libraries(segv_test_include segv)
