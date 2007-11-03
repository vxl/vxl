# This is /db3p/expatpp

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/expatpp/expat/lib )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/expatpp/src_pp )
#define COMPILED_FROM_DSP

SET(test_sources
    SimpleTest.cpp     
    StdAfx.cpp  StdAfx.h
)

 ADD_EXECUTABLE(simpletest ${test_sources})

 TARGET_LINK_LIBRARIES(simpletest expatpp)


 
