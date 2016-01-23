# This is brl/b3p/expatpp/SimpleTest

include_directories( ${BRL_INCLUDE_DIR}/expatpp/expat/lib )
include_directories( ${BRL_INCLUDE_DIR}/expatpp/src_pp )
#define COMPILED_FROM_DSP

set(test_sources
    SimpleTest.cpp
)

 add_executable(simpletest ${test_sources})

 target_link_libraries(simpletest expatpp)



