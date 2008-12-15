# This is brl/b3p/expatpp/SimpleTest

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/expatpp/expat/lib )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/expatpp/src_pp )
#define COMPILED_FROM_DSP

SET(test_sources
    SimpleTest.cpp     
)

 ADD_EXECUTABLE(simpletest ${test_sources})

 TARGET_LINK_LIBRARIES(simpletest expatpp)


 
