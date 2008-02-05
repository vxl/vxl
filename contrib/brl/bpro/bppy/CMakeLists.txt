# brl/bpro/bppy/CMakeLists.txt
# ==== NEED TO SETUP FIND SCRIPT ===
SET(HAS_PYTHON FALSE)
IF(HAS_PYTHON)
INCLUDE_DIRECTORIES( c:/Python-2.5.1/Include )
INCLUDE_DIRECTORIES( c:/Python-2.5.1/PC )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
LINK_DIRECTORIES(C:/Python-2.5.1/PCbuild8/Win32)

SET(batch_sources
    batch.h  batch.cxx
    reg.cxx
 )
ADD_LIBRARY(batch SHARED ${batch_sources})
SET_TARGET_PROPERTIES(batch PROPERTIES OUTPUT_NAME batch_d SUFFIX .pyd)
TARGET_LINK_LIBRARIES(batch bprb brdb vbl python25_d)
ENDIF(HAS_PYTHON)
